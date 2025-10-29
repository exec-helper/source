use anyhow::{Context, Result, anyhow, bail};
use clap::{Arg, ArgMatches, Args, FromArgMatches};
use console::{Term, style};
use indicatif::{ProgressBar, ProgressStyle};
use itertools::Itertools;
use lets_find_up::{FindUpKind, FindUpOptions, find_up_with};
use serde::Deserialize;
use serde_json::{Value, from_value};
use serde_saphyr::from_reader;
use std::collections::HashMap;
use std::env;
use std::fmt::Arguments;
use std::fs::File;
use std::io::Write;
use std::iter::zip;
use std::path::PathBuf;
use std::process::ExitStatus;
use std::sync::Arc;
use std::sync::atomic::{AtomicUsize, Ordering};
use subst::substitute;
use tokio::process::Command;
use tokio_process_tools::{LineParsingOptions, Next, Process};
use tracing::{Level, info, trace};
use tracing_subscriber::FmtSubscriber;

type Environment = HashMap<String, String>;
type PatternReference = String;
type PatternReferences = Vec<PatternReference>;
type PatternValues = HashMap<String, Vec<String>>;

#[derive(Debug, Deserialize)]
#[serde(untagged)]
enum RootCommand {
    Command(Vec<String>),

    Plugin(Value),
}

#[derive(Clone, Debug, Deserialize)]
#[serde(untagged)]
enum CommandLine {
    SingleCommand(Vec<String>),

    MultipleCommands(Vec<HashMap<String, Vec<String>>>),
}

#[derive(Debug, Deserialize)]
#[serde(rename_all = "kebab-case")]
struct Pattern {
    default_values: Vec<String>,
    short_option: Option<char>,
    long_option: Option<String>,
}
type Patterns = HashMap<String, Pattern>;

#[derive(Debug, Deserialize)]
struct Config {
    //commands: HashMap<String, String>,
    patterns: Option<Patterns>,

    #[serde(flatten)]
    root_keys: HashMap<String, RootCommand>,
}

#[derive(Clone, Debug, Deserialize)]
#[serde(rename_all = "kebab-case")]
struct CommandLineCommand {
    patterns: Option<PatternReferences>,
    environment: Option<Environment>,
    command_line: Option<CommandLine>,
    _working_dir: Option<String>,

    #[serde(flatten)]
    commands: Option<HashMap<String, CommandLineCommand>>,
}

#[derive(Args, Debug)]
#[command(version, about, long_about = None)]
struct Cli {
    commands: Vec<String>,

    #[arg(short = 's', long)]
    configuration_file: Option<PathBuf>,

    #[arg(short = 'n', long, default_value_t = false)]
    dry_run: bool,

    #[arg(short, long, default_value_t = Level::WARN)]
    log_level: Level,

    #[arg(long, default_value_t = false)]
    keep_output: bool,

    #[arg(short, long, default_value_t = false)]
    keep_going: bool,

    #[arg(long, default_value_t = false)]
    non_interactive: bool,

    #[arg(long)]
    force_color: Option<bool>,
}

struct ExecutionCommand {
    patterns: Option<PatternReferences>,
    environment: Option<Environment>,
    command: Vec<String>,
}

pub fn user_info_fmt(args: Arguments) {
    let line = style(args).cyan().bold();
    println!("{line}");
}

#[macro_export]
macro_rules! user_info {
    ($($arg:tt)*) => {
        $crate::user_info_fmt(format_args!($($arg)*))
    };
}

pub fn user_success_fmt(args: Arguments) {
    let line = style(args).green().bold();
    println!("{line}");
}

#[macro_export]
macro_rules! user_success {
    ($($arg:tt)*) => {
        $crate::user_success_fmt(format_args!($($arg)*))
    };
}

pub fn user_warn_fmt(args: Arguments) {
    let line = style(args).yellow().bold();
    eprintln!("{line}");
}

#[macro_export]
macro_rules! user_warn {
    ($($arg:tt)*) => {
        $crate::user_warn_fmt(format_args!($($arg)*))
    };
}

pub fn user_error_fmt(args: Arguments) {
    let line = style(args).red().bold();
    eprintln!("{line}");
}

#[macro_export]
macro_rules! user_error {
    ($($arg:tt)*) => {
        $crate::user_error_fmt(format_args!($($arg)*))
    };
}

fn run_command_line_command(
    command: &String,
    plugin_config: Value,
) -> Result<Vec<ExecutionCommand>> {
    trace!("Processing subconfig as a command-line-command plugin...");
    let config: CommandLineCommand = match from_value(plugin_config) {
        Ok(config) => config,
        Err(e) => bail!(
            "Failed to parse command-line-command plugin at path '{}'",
            e.path()
        ),
    };

    let command_config = match config.commands {
        Some(ref commands) => match commands.get(command) {
            Some(config) => config.clone(),
            None => config.clone(),
        },
        None => config.clone(),
    };

    let command_line = match command_config.command_line {
        Some(command) => command,
        None => match config.command_line {
            Some(command) => command,
            None => bail!(
                "You must define a command-line option for command '{}' below the command-line-command config",
                command
            ),
        },
    };

    let environment = command_config.environment.or(config.environment);
    let patterns = command_config.patterns.or(config.patterns);

    match command_line {
        CommandLine::SingleCommand(command) => Ok(vec![ExecutionCommand {
            command,
            environment,
            patterns,
        }]),
        CommandLine::MultipleCommands(commands) => Ok(commands
            .into_iter()
            .flat_map(|command| {
                command
                    .into_values()
                    .map(|command| ExecutionCommand {
                        command,
                        environment: environment.clone(),
                        patterns: patterns.clone(),
                    })
                    .collect::<Vec<ExecutionCommand>>()
            })
            .collect::<Vec<ExecutionCommand>>()),
    }
}

fn build_subcommand(
    command: &String,
    subcommands: &[String],
    config: &Config,
) -> Result<Vec<ExecutionCommand>> {
    subcommands
        .iter()
        .map(|subcommand| {
            trace!(
                "Redirecting to subcommand '{}' for command '{}'",
                subcommand, command
            );
            build_execution_command(command, subcommand, config)
        })
        .try_fold(Vec::new(), |mut acc, cur| {
            cur.map(|inner| {
                acc.extend(inner);
                acc
            })
        })
}

fn build_plugin(
    command: &String,
    plugin: &String,
    plugin_config: &Value,
) -> Result<Vec<ExecutionCommand>> {
    trace!("Using plugin '{}' for command '{}'", plugin, command);
    match plugin.as_str() {
        "command-line-command" => run_command_line_command(command, plugin_config.clone()),
        &_ => Err(anyhow!("Invalid plugin: '{}'!", plugin)),
    }
}

fn build_execution_command(
    parent: &String,
    command: &String,
    config: &Config,
) -> Result<Vec<ExecutionCommand>> {
    match config.root_keys.get(command) {
        Some(subconfig) => match subconfig {
            RootCommand::Command(subcommands) => build_subcommand(command, subcommands, config),
            RootCommand::Plugin(plugin_config) => build_plugin(parent, command, plugin_config),
        },
        None => Err(anyhow!(
            "'{}' not found in the exec-helper configuration!",
            command
        )),
    }
}

fn build_env(env_options: &Option<Environment>) -> Result<HashMap<String, String>> {
    // Start with a copy of the current environment.
    let mut env_map: HashMap<String, String> = env::vars_os()
        .filter_map(|(k_os, v_os)| {
            match (k_os.clone().into_string(), v_os.clone().into_string()) {
                (Ok(k), Ok(v)) => Some((k, v)),
                // Silently drop malformed entries; you could log here if you wish.
                _ => {
                    user_warn!(
                        "Failed to convert '{:?}' or '{:?}' to a valid UTF-8 string. Ignoring it!",
                        k_os,
                        v_os
                    );
                    None
                }
            }
        })
        .collect();

    if let Some(extra_vars) = env_options {
        for (k, v) in extra_vars {
            env_map.insert(k.clone(), v.clone());
        }
    }

    Ok(env_map)
}

fn find_config(file: &str) -> Result<PathBuf> {
    let options = FindUpOptions {
        kind: FindUpKind::File,
        ..Default::default()
    };

    find_up_with(file, options)
        .context("Failed to traverse up directories to find the configuration file")?
        .ok_or(anyhow!("Failed to find a configuration file!"))
}

fn read_config(configuration_file: Option<PathBuf>) -> Result<(PathBuf, Config)> {
    let config_file_path = match configuration_file {
        Some(file) => file,
        None => find_config(".exec-helper")?,
    };

    trace!("Using config '{:?}'", config_file_path);
    let config_file = File::open(&config_file_path)
        .with_context(|| format!("Failed to open {}!", config_file_path.display()))?;

    let config = Config::deserialize(from_reader(config_file))
        .context("Failed to parse the provided exec-helper configuration!")?;

    Ok((config_file_path, config))
}

fn find_and_read_config() -> Result<(PathBuf, Config)> {
    let temp_cli = Cli::augment_args(
        clap::Command::new("exec-helper")
            .ignore_errors(true)
            .disable_help_flag(true)
            .disable_help_subcommand(true),
    );

    let matches = temp_cli.get_matches();

    let temp_fixed_cli =
        Cli::from_arg_matches(&matches).context("Invalid command line arguments!")?;

    read_config(temp_fixed_cli.configuration_file).context("Failed to read configuration file!")
}

fn handle_cli_arguments(patterns: &Option<Patterns>) -> Result<(ArgMatches, Cli)> {
    let mut cli = clap::Command::new("exec-helper");

    cli = Cli::augment_args(cli);

    if let Some(actual_patterns) = patterns {
        for (key, pattern) in actual_patterns {
            let mut pattern_argument = Arg::new(clap::Id::from(key))
                .help(format!("Values for pattern '{}'", key))
                .default_values(pattern.default_values.clone())
                .num_args(1..);

            if let Some(short_option) = pattern.short_option {
                pattern_argument = pattern_argument.short(short_option);
            }

            if let Some(long_option) = &pattern.long_option {
                pattern_argument = pattern_argument.long(long_option);
            }

            cli = cli.arg(pattern_argument);
        }
    }

    let matches = cli.get_matches();

    let fixed_cli = Cli::from_arg_matches(&matches).context("Invalid command line arguments!")?;

    Ok((matches, fixed_cli))
}

#[tokio::main]
async fn main() -> Result<()> {
    let term = Term::stdout();

    let (root_dir, config) = find_and_read_config()?;

    let (dynamic_cli, fixed_cli) = handle_cli_arguments(&config.patterns)?;

    if let Some(force_color) = fixed_cli.force_color {
        console::set_colors_enabled(force_color);
    }

    let subscriber = FmtSubscriber::builder()
        .with_max_level(fixed_cli.log_level)
        .finish();

    tracing::subscriber::set_global_default(subscriber)
        .context("Setting default subscriber failed")?;

    info!("Starting exec-helper...");

    let mut pattern_values: PatternValues = match config.patterns {
        Some(ref patterns) => patterns
            .iter()
            .map(|(key, pattern)| {
                let values = match dynamic_cli.try_get_many::<String>(key) {
                    Ok(values) => match values {
                        Some(values) => values.cloned().collect::<Vec<_>>(),
                        None => pattern.default_values.clone(),
                    },
                    Err(_) => pattern.default_values.clone(),
                };

                (key.clone(), values.clone())
            })
            .collect(),
        None => HashMap::<String, Vec<String>>::new(),
    };

    // TODO: replace default values with values defined on the CLI
    pattern_values.insert(
        "EH_ROOT_DIR".to_string(),
        vec![
            root_dir
                .parent()
                .ok_or(anyhow!(
                    "Config file somehow does not have a parent directory!"
                ))?
                .to_str()
                .ok_or(anyhow!("Failed to convert root dir to valid UTF-8!"))?
                .to_string(),
        ],
    );
    trace!("Pattern values = {:?}", pattern_values);

    let clis = fixed_cli
        .commands
        .iter()
        .map(|command| {
            build_execution_command(command, command, &config).with_context(|| {
                format!(
                    "Failed to generate execution instructions for command '{}'",
                    command
                )
            })
        })
        .try_fold(Vec::new(), |mut acc, cur| {
            cur.map(|inner| {
                acc.extend(inner);
                acc
            })
        })?;

    let no_patterns: Vec<String> = vec!["NO_PATTERNS_DEFINED".to_string()];

    let mut last_error_status = ExitStatus::default();
    for cli in clis {
        let pattern_iterator_result = match cli.patterns {
            Some(ref patterns) => patterns
                .iter()
                .map(|pattern| {
                    pattern_values
                        .get(pattern)
                        .with_context(|| {
                            format!("Undefined pattern used in command: '{}'", pattern)
                        })
                        .map(|v| v.iter())
                })
                .collect(),

            None => Ok(vec![no_patterns.iter()]), // If no patterns defined, iterate once
        };
        let pattern_iterator = pattern_iterator_result.context("Failed to resolve pattern!")?;

        for pattern_combination in pattern_iterator.into_iter().multi_cartesian_product() {
            let environment = build_env(&cli.environment)
                .context("Failed to construct the environment for the command!")?;

            let commands: Vec<String> = cli
                .command
                .iter()
                .map(|command| {
                    let mut substituted = command.clone();
                    for (pattern_key, pattern_value) in zip(
                        cli.patterns.as_ref().unwrap_or(&Vec::<String>::new()),
                        pattern_combination.clone(),
                    ) {
                        info!(
                            "Going to substitute '{}' patterns with '{}'",
                            pattern_key, pattern_value
                        );
                        let needle = format!("{{{{ {} }}}}", pattern_key);
                        substituted = substituted.replace(&needle, pattern_value);
                    }
                    match substitute(&substituted, &environment) {
                        Ok(result) => result,
                        Err(e) => {
                            user_warn!(
                                "Failed to substitute environment variable in '{}': {:?}",
                                command,
                                e
                            );
                            command.clone()
                        }
                    }
                })
                .collect();

            let substituted_command = commands
                .iter()
                .map(String::as_str)
                .collect::<Vec<_>>()
                .join(" ");

            user_info!("..  Execute '{substituted_command}'...");

            if fixed_cli.dry_run {
                continue;
            }

            let pb = match fixed_cli.non_interactive {
                true => Arc::new(ProgressBar::hidden()),
                false => Arc::new(ProgressBar::new_spinner()),
            };
            pb.set_style(
                ProgressStyle::default_spinner()
                    .template("[{elapsed}] {spinner} {msg:.cyan}")
                    .unwrap(),
            );
            pb.set_message(format!("Running '{}'...", &substituted_command));
            pb.tick();

            let number_of_output_lines = Arc::new(AtomicUsize::new(1));

            let mut cmd = Command::new(&commands[0]);
            cmd.args(&commands[1..]);
            cmd.envs(environment);

            let mut process = Process::new(cmd).spawn_single_subscriber().unwrap();

            let stdout_poke = pb.clone();
            let stdout_nb_of_output_lines = number_of_output_lines.clone();
            let _stdout_monitor = process.stdout().inspect_lines(
                move |line| {
                    stdout_nb_of_output_lines.fetch_add(1, Ordering::Relaxed);
                    if stdout_poke.is_hidden() {
                        eprintln!("    {line}");
                    } else {
                        stdout_poke.println(format!("    {line}"));
                        stdout_poke.tick();
                    }
                    std::io::stderr().flush().ok();
                    Next::Continue
                },
                LineParsingOptions::default(),
            );

            let stderr_poke = pb.clone();
            let stderr_nb_of_output_lines = number_of_output_lines.clone();
            let _stderr_monitor = process.stderr().inspect_lines(
                move |line| {
                    stderr_nb_of_output_lines.fetch_add(1, Ordering::Relaxed);
                    if stderr_poke.is_hidden() {
                        eprintln!("!   {line}");
                    } else {
                        stderr_poke.println(format!("!    {line}"));
                        stderr_poke.tick();
                    }
                    std::io::stderr().flush().ok();
                    Next::Continue
                },
                LineParsingOptions::default(),
            );

            let status = process
                .wait_for_completion_or_terminate(
                    std::time::Duration::from_secs(3600),
                    std::time::Duration::from_secs(5),
                    std::time::Duration::from_secs(5),
                )
                .await
                .context("Process is terminated abnormaly")?;

            std::io::stderr().flush().ok();
            pb.finish_and_clear();
            std::io::stderr().flush().ok();

            if status.success() {
                info!("Process exited with exit code 0 (success!)");

                if !fixed_cli.keep_output {
                    term.clear_last_lines(number_of_output_lines.load(Ordering::Relaxed))
                        .context("Failed to erase last lines")?;
                }

                user_success!("OK  Execute '{substituted_command}'");
            } else {
                user_error!("ERR Execute '{substituted_command}'!");

                if last_error_status.success() {
                    last_error_status = status;
                }

                if !fixed_cli.keep_going {
                    bail!("Process exited with non-zero exit code: {}", status);
                }
            }
        }
    }
    if last_error_status.success() {
        user_success!("All commands were executed successfully!");
    } else {
        user_error!("There were errors while executing commands!");
    }

    Ok(())
}
