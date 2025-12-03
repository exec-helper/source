use anyhow::Result;
use console::style;
use serde::Deserialize;
use std::collections::HashMap;
use std::fmt::Arguments;

pub type Environment = HashMap<String, String>;
pub type PatternReference = String;
pub type PatternReferences = Vec<PatternReference>;

#[derive(Clone, Debug, Default, PartialEq)]
pub struct ExecutionCommand {
    pub patterns: Option<PatternReferences>,
    pub environment: Option<Environment>,
    pub command: Vec<String>,
    pub working_directory: String,
}

// Make sure serde handles primitive YAML or JSON types, even though we will just treat them as strings later
#[derive(Clone, Debug, Deserialize)]
#[serde(untagged)]
pub enum CommandLineArgument {
    String(String),
    Integer(i64),
    Float(f64),
}

impl std::fmt::Display for CommandLineArgument {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            CommandLineArgument::String(s) => write!(f, "{s}"),
            CommandLineArgument::Integer(i) => write!(f, "{i}"),
            CommandLineArgument::Float(v) => write!(f, "{v}"),
        }
    }
}

pub fn deserialize_arguments(command: &[CommandLineArgument]) -> Result<Vec<String>> {
    let args: Vec<String> = command.iter().map(|value| value.to_string()).collect();
    Ok(args)
}

pub fn user_info_fmt(args: Arguments) {
    let line = style(args).cyan().bold();
    println!("{line}");
}

#[macro_export]
macro_rules! user_info {
    ($($arg:tt)*) => {
        $crate::common::user_info_fmt(format_args!($($arg)*))
    };
}

pub fn user_success_fmt(args: Arguments) {
    let line = style(args).green().bold();
    println!("{line}");
}

#[macro_export]
macro_rules! user_success {
    ($($arg:tt)*) => {
        $crate::common::user_success_fmt(format_args!($($arg)*))
    };
}

pub fn user_warn_fmt(args: Arguments) {
    let line = style(args).yellow().bold();
    eprintln!("{line}");
}

#[macro_export]
macro_rules! user_warn {
    ($($arg:tt)*) => {
        $crate::common::user_warn_fmt(format_args!($($arg)*))
    };
}

pub fn user_error_fmt(args: Arguments) {
    let line = style(args).red().bold();
    eprintln!("{line}");
}

#[macro_export]
macro_rules! user_error {
    ($($arg:tt)*) => {
        $crate::common::user_error_fmt(format_args!($($arg)*))
    };
}
