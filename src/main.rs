use anyhow::{Context, Result, anyhow, bail};
use clap::{Arg, ArgMatches, Args, FromArgMatches};
use console::Term;
use indicatif::{ProgressBar, ProgressStyle};
use itertools::Itertools;
use lets_find_up::{FindUpKind, FindUpOptions, find_up_with};
use notify::Watcher;
use serde::Deserialize;
use serde_json::{Value, from_value};
use serde_saphyr::from_reader;
use std::collections::HashMap;
use std::env;
use std::ffi::OsStr;
use std::fs::{File, read_dir};
use std::io::Write;
use std::iter::zip;
use std::path::{Path, PathBuf};
use std::process::ExitCode;
use std::sync::Arc;
use std::sync::atomic::{AtomicUsize, Ordering};
use subst::substitute;
use tokio::process::Command;
use tokio_process_tools::{LineParsingOptions, Next, Process};
use tracing::{Level, info, trace};
use tracing_subscriber::FmtSubscriber;

mod common;
mod lua;

use crate::common::{CommandLineArgument, Environment, ExecutionCommand, PatternReferences};
use crate::lua::run_lua_plugin;

type PatternValues = HashMap<String, Vec<String>>;
type Plugins = HashMap<String, Plugin>;

#[derive(Debug, Deserialize)]
#[serde(untagged)]
enum RootCommand {
    Command(Vec<String>),

    Plugin(Value),
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
    commands: HashMap<String, String>,
    patterns: Option<Patterns>,
    watch: Option<Vec<String>>,

    #[serde(flatten)]
    root_keys: HashMap<String, RootCommand>,
}

#[derive(Clone, Debug, Deserialize)]
#[serde(rename_all = "kebab-case")]
struct CommandLineCommand {
    patterns: Option<PatternReferences>,
    environment: Option<Environment>,
    command_line: Option<CommandLine>,
    working_dir: Option<String>,

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

    #[arg(long, visible_alias = "debug", default_value_t = Level::WARN)]
    log_level: Level,

    #[arg(long, default_value_t = false)]
    keep_output: bool,

    #[arg(short, long, default_value_t = false)]
    keep_going: bool,

    #[arg(long, default_value_t = false)]
    non_interactive: bool,

    #[arg(long)]
    force_color: Option<bool>,

    #[arg(long, default_value_t = false)]
    list_plugins: bool,

    #[arg(short, long, default_value_t = false)]
    watch: bool,
}

type PluginGenerateCommandFn = Box<dyn Fn(&String, Value) -> Result<Vec<ExecutionCommand>>>;

struct Plugin {
    name: String,
    description: String,
    generate_command: PluginGenerateCommandFn,
}

pub fn resolve_working_directory(working_directory: &String, root_dir: &Path) -> PathBuf {
    let path = PathBuf::from(working_directory);
    match path.is_absolute() {
        true => path,
        false => root_dir.join(path),
    }
}

#[cfg(unix)]
fn signal_process_group(pid: u32, signal: nix::sys::signal::Signal) {
    use nix::sys::signal::killpg;
    use nix::unistd::Pid;
    let _ = killpg(Pid::from_raw(pid as i32), signal);
}

async fn terminate_child_process(
    process: &mut tokio_process_tools::ProcessHandle<impl tokio_process_tools::OutputStream>,
) {
    let child_pid = process.id();

    // Send SIGINT to the entire process group first, so grandchildren
    // (e.g. the binary spawned by `cargo run`) also receive the signal.
    #[cfg(unix)]
    if let Some(pid) = child_pid {
        signal_process_group(pid, nix::sys::signal::Signal::SIGINT);
    }

    // Let the library handle the escalation (SIGINT→SIGTERM→SIGKILL)
    // and lifecycle management for the direct child.
    match process
        .terminate(
            std::time::Duration::from_secs(5),
            std::time::Duration::from_secs(5),
        )
        .await
    {
        Ok(_) => {}
        Err(e) => user_error!("Failed to gracefully terminate child process: {e}"),
    }

    // SIGKILL any remaining grandchildren in the process group.
    #[cfg(unix)]
    if let Some(pid) = child_pid {
        signal_process_group(pid, nix::sys::signal::Signal::SIGKILL);
    }
}

enum RunOutcome {
    Completed(ExitCode),
    Interrupted,
    Restarting,
}

#[derive(Clone, Debug, Deserialize)]
#[serde(untagged)]
pub enum CommandLine {
    SingleCommand(Vec<CommandLineArgument>),

    MultipleCommands(Vec<HashMap<String, Vec<CommandLineArgument>>>),
}

fn run_command_line_command(
    command: &String,
    plugin_config: Value,
) -> Result<Vec<ExecutionCommand>> {
    trace!("Resolved '{}' -> command-line-command plugin", command);
    let config: CommandLineCommand = match from_value(plugin_config) {
        Ok(config) => config,
        Err(e) => bail!(
            "Failed to parse command-line-command plugin at path '{}'",
            e.path()
        ),
    };
    trace!("Selected config = {:?}", config);

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
    let working_directory = command_config
        .working_dir
        .or(config.working_dir)
        .unwrap_or(".".to_string());

    match command_line {
        CommandLine::SingleCommand(command) => Ok(vec![ExecutionCommand {
            command: command.into_iter().map(|arg| arg.to_string()).collect(),
            environment,
            patterns,
            working_directory,
        }]),
        CommandLine::MultipleCommands(commands) => Ok(commands
            .into_iter()
            .flat_map(|command| {
                command
                    .into_values()
                    .map(|command| ExecutionCommand {
                        command: command.into_iter().map(|arg| arg.to_string()).collect(),
                        environment: environment.clone(),
                        patterns: patterns.clone(),
                        working_directory: working_directory.clone(),
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
    plugins: &Plugins,
) -> Result<Vec<ExecutionCommand>> {
    subcommands
        .iter()
        .map(|subcommand| {
            trace!("Redirecting '{}' -> '{}'", command, subcommand);
            build_execution_command(command, subcommand, config, plugins)
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
    plugin_name: &String,
    plugin_config: &Value,
    plugins: &Plugins,
) -> Result<Vec<ExecutionCommand>> {
    trace!("Using '{}' -> '{}'", command, plugin_name);

    let plugin = plugins
        .get(plugin_name)
        .with_context(|| format!("Could not find plugin '{}'!", plugin_name))?;

    let generate_command = &plugin.generate_command;

    generate_command(command, plugin_config.clone())
}

fn build_execution_command(
    parent: &String,
    command: &String,
    config: &Config,
    plugins: &Plugins,
) -> Result<Vec<ExecutionCommand>> {
    match config.root_keys.get(command) {
        Some(subconfig) => match subconfig {
            RootCommand::Command(subcommands) => {
                build_subcommand(command, subcommands, config, plugins)
            }
            RootCommand::Plugin(plugin_config) => {
                build_plugin(parent, command, plugin_config, plugins)
            }
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

fn builtin_plugins() -> Result<Plugins> {
    let mut plugins = HashMap::new();
    plugins.insert(
        "command-line-command".to_string(),
        Plugin {
            name: "command-line-command".to_string(),
            description: "Command-line-command (internal)".to_string(),
            generate_command: Box::new(|command: &String, config: Value| {
                run_command_line_command(command, config)
            }),
        },
    );

    Ok(plugins)
}

fn find_plugins(search_paths: &[PathBuf]) -> Result<Plugins> {
    let default_plugins = builtin_plugins().context("Failed to load built-in plugins!")?;

    let plugins = search_paths
        .iter()
        .flat_map(|dir: &PathBuf| {
            read_dir(dir)
                .into_iter()
                .flatten()
                .filter_map(Result::ok)
                .map(|entry| entry.path())
                .filter(|p| p.is_file())
                .filter(|p| p.extension() == Some(OsStr::new("lua")))
        })
        .filter_map(|path| {
            let plugin_path = path.clone();
            let plugin_name = path.file_stem()?.to_str()?;
            let plugin = Plugin {
                name: plugin_name.to_string(),
                description: format!("Lua plugin at '{}'", path.display()),
                generate_command: Box::new(move |command: &String, config: Value| {
                    run_lua_plugin(command, config, plugin_path.clone())
                }),
            };
            Some(plugin)
        })
        .fold(default_plugins, |mut acc, plugin| {
            acc.insert(plugin.name.clone(), plugin);
            acc
        });

    trace!("Plugins = {:?}", plugins.keys());

    Ok(plugins)
}

fn handle_cli_arguments(
    commands: &HashMap<String, String>,
    patterns: &Option<Patterns>,
) -> Result<(ArgMatches, Cli)> {
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

    let commands_help = commands
        .iter()
        .map(|(command, description)| format!(" {:<25} {}", command, description))
        .fold("Commands:\n".to_string(), |mut acc, line| {
            acc.push_str(&line);
            acc.push('\n');
            acc
        });

    cli = cli.after_help(commands_help);

    let matches = cli.get_matches();

    let fixed_cli = Cli::from_arg_matches(&matches).context("Invalid command line arguments!")?;

    Ok((matches, fixed_cli))
}

async fn run_cli(
    clis: &Vec<ExecutionCommand>,
    pattern_values: &PatternValues,
    fixed_cli: &Cli,
    term: &Term,
    root_dir: &Path,
    mut restart_rx: Option<&mut tokio::sync::mpsc::Receiver<notify::Event>>,
) -> Result<RunOutcome> {
    let no_patterns: Vec<String> = vec!["NO_PATTERNS_DEFINED".to_string()];
    let mut last_error_status = ExitCode::from(0);

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
                        let needle = format!("{{{}}}", pattern_key);
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

            user_info!("[EXEC] {substituted_command}");

            if fixed_cli.dry_run {
                continue;
            }

            let pb = match fixed_cli.non_interactive {
                true => Arc::new(ProgressBar::hidden()),
                false => Arc::new(ProgressBar::new_spinner()),
            };
            pb.set_style(
                ProgressStyle::default_spinner()
                    .template("[running] {spinner} {msg:.cyan}")
                    .unwrap(),
            );
            pb.set_message(substituted_command.clone());
            pb.tick();

            let number_of_output_lines = Arc::new(AtomicUsize::new(1));

            let mut cmd = Command::new(&commands[0]);
            cmd.args(&commands[1..]);
            cmd.envs(environment);

            let working_directory = resolve_working_directory(&cli.working_directory, root_dir);
            trace!("Executing this in dir {}", working_directory.display());
            cmd.current_dir(working_directory);

            // Put the child in its own process group so we can signal the
            // entire tree (including grandchildren) on Ctrl+C.
            #[cfg(unix)]
            unsafe {
                cmd.pre_exec(|| {
                    nix::unistd::setpgid(
                        nix::unistd::Pid::from_raw(0),
                        nix::unistd::Pid::from_raw(0),
                    )
                    .map_err(std::io::Error::other)
                });
            }

            let mut process = Process::new(cmd)
                .spawn_single_subscriber()
                .context("Failed to spawn process!")?;

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

            let status = tokio::select! {
                biased;
                _ = tokio::signal::ctrl_c() => {
                    std::io::stderr().flush().ok();
                    pb.finish_and_clear();
                    user_error!("[ INT] Interrupted! Terminating child process...");
                    terminate_child_process(&mut process).await;
                    return Ok(RunOutcome::Interrupted);
                }
                _ = async {
                    if let Some(ref mut rx) = restart_rx {
                        let _ = rx.recv().await;
                    } else {
                        std::future::pending::<()>().await;
                    }
                } => {
                    std::io::stderr().flush().ok();
                    pb.finish_and_clear();
                    terminate_child_process(&mut process).await;
                    return Ok(RunOutcome::Restarting);
                }
                status = process.wait_for_completion_or_terminate(
                    std::time::Duration::from_secs(3600),
                    std::time::Duration::from_secs(5),
                    std::time::Duration::from_secs(5),
                ) => {
                    status.context("Process is terminated abnormaly")?
                }
            };

            std::io::stderr().flush().ok();
            pb.finish_and_clear();
            std::io::stderr().flush().ok();

            if status.success() {
                info!("Process exited with exit code 0 (success!)");

                if !fixed_cli.keep_output {
                    term.clear_last_lines(number_of_output_lines.load(Ordering::Relaxed))
                        .context("Failed to erase last lines")?;
                }

                user_success!("[ OK ]  {substituted_command}");
            } else {
                user_error!("[ERR ] {substituted_command}!");
                user_error!("Process exited with {status}!");

                last_error_status = match status.code() {
                    Some(code) => ExitCode::from(code as u8),
                    None => ExitCode::from(1),
                };

                if !fixed_cli.keep_going {
                    return Ok(RunOutcome::Completed(last_error_status));
                }
            }
        }
    }
    Ok(RunOutcome::Completed(last_error_status))
}

#[tokio::main]
async fn main() -> Result<()> {
    let term = Term::stdout();

    let (config_file, config) = find_and_read_config()?;

    let (dynamic_cli, fixed_cli) = handle_cli_arguments(&config.commands, &config.patterns)?;

    if let Some(force_color) = fixed_cli.force_color {
        console::set_colors_enabled(force_color);
    }

    let subscriber = FmtSubscriber::builder()
        .with_max_level(fixed_cli.log_level)
        .finish();

    tracing::subscriber::set_global_default(subscriber)
        .context("Setting default subscriber failed")?;

    info!("Starting exec-helper...");
    let root_dir = config_file.parent().ok_or(anyhow!(
        "Config file somehow does not have a parent directory!"
    ))?;

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

    pattern_values.insert(
        "EH_ROOT_DIR".to_string(),
        vec![
            root_dir
                .to_str()
                .ok_or(anyhow!("Failed to convert root dir to valid UTF-8!"))?
                .to_string(),
        ],
    );
    trace!("Pattern values = {:?}", pattern_values);

    let plugin_default_search_path: PathBuf = PathBuf::from(env!("PLUGIN_DEFAULT_SEARCH_PATH"));

    let plugins = find_plugins(&[plugin_default_search_path]).context("Failed to find plugins!")?;

    if fixed_cli.list_plugins {
        for (name, plugin) in plugins {
            user_info!("{:.<25} {}", name, plugin.description);
        }
        return Ok(());
    }

    let clis = fixed_cli
        .commands
        .iter()
        .map(|command| {
            trace!("Resolving command '{}'...", command);
            build_execution_command(command, command, &config, &plugins).with_context(|| {
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

    let return_code = if fixed_cli.watch {
        let (tx, mut rx) = tokio::sync::mpsc::channel::<notify::Event>(10);
        let mut watcher =
            notify::recommended_watcher(move |response: notify::Result<notify::Event>| {
                if let Ok(event) = response
                    && let notify::EventKind::Access(access) = event.kind
                    && let notify::event::AccessKind::Close(close) = access
                    && close == notify::event::AccessMode::Write
                {
                    tx.blocking_send(event).unwrap();
                }
            })
            .context("Failed to watch the configured files")?;

        match config.watch {
            Some(watch) => {
                for path in watch {
                    info!("Watching {path} for changes...");
                    watcher.watch(Path::new(&path), notify::RecursiveMode::Recursive)?;
                }
            }
            None => bail!(
                "You did not configure any files to watch in the configuration file! List the files you want to watch as a list of directories and/or files"
            ),
        };

        loop {
            match run_cli(
                &clis,
                &pattern_values,
                &fixed_cli,
                &term,
                root_dir,
                Some(&mut rx),
            )
            .await
            {
                Ok(RunOutcome::Restarting) => {
                    while rx.try_recv().is_ok() {}
                    user_info!("[INFO] Change detected! Rerunning commands...");
                }
                result @ Ok(RunOutcome::Interrupted) => break result,
                other => {
                    // Completed (success or error): wait for next file change or Ctrl+C
                    tokio::select! {
                        biased;
                        _ = tokio::signal::ctrl_c() => {
                            break Ok(RunOutcome::Interrupted);
                        }
                        event = rx.recv() => {
                            match event {
                                Some(_) => {
                                    while rx.try_recv().is_ok() {}
                                    user_info!("[INFO] Change detected! Rerunning commands...");
                                }
                                None => break other,
                            }
                        }
                    }
                }
            }
        }
    } else {
        run_cli(&clis, &pattern_values, &fixed_cli, &term, root_dir, None).await
    };

    match return_code {
        Ok(RunOutcome::Completed(ExitCode::SUCCESS)) => {
            user_success!("All commands were executed successfully!");
            Ok(())
        }
        Ok(RunOutcome::Interrupted) => Err(anyhow!("Interrupted!")),
        Ok(RunOutcome::Completed(_)) => Err(anyhow!("There were errors while executing commands!")),
        Ok(RunOutcome::Restarting) => Err(anyhow!(
            "Internal error: unexpected restart outside of watch loop!"
        )),
        Err(_) => Err(anyhow!("There were errors while executing commands!")),
    }
}
