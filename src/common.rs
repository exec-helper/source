use anyhow::Result;
use console::style;
use serde::{Deserialize, Deserializer, de};
use std::collections::HashMap;
use std::fmt::{Arguments, Formatter};

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
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
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

pub struct VerboseVisitor;

impl<'de> de::Visitor<'de> for VerboseVisitor {
    type Value = bool;

    fn expecting(&self, formatter: &mut Formatter) -> std::fmt::Result {
        formatter.write_str("a boolean, a truthy string like \"yes\", a number, or null")
    }

    fn visit_bool<E>(self, v: bool) -> Result<bool, E> {
        Ok(v)
    }

    fn visit_str<E>(self, v: &str) -> Result<bool, E>
    where
        E: de::Error,
    {
        Ok(matches!(
            v.trim().to_ascii_lowercase().as_str(),
            "yes" | "y" | "true" | "1"
        ))
    }

    fn visit_string<E>(self, v: String) -> Result<bool, E>
    where
        E: de::Error,
    {
        self.visit_str(&v)
    }

    fn visit_i64<E>(self, v: i64) -> Result<bool, E>
    where
        E: de::Error,
    {
        Ok(v != 0)
    }

    fn visit_u64<E>(self, v: u64) -> Result<bool, E>
    where
        E: de::Error,
    {
        Ok(v != 0)
    }

    fn visit_f64<E>(self, v: f64) -> Result<bool, E>
    where
        E: de::Error,
    {
        Ok(v != 0.0)
    }

    fn visit_none<E>(self) -> Result<bool, E>
    where
        E: de::Error,
    {
        // `null` should be interpreted as “no value”. By returning an error
        // we let the outer `deserialize_verbose_opt` turn it into `None`.
        Err(E::invalid_type(de::Unexpected::Unit, &self))
    }

    // ── option (covers the case where the JSON value itself is an Option) ---
    fn visit_some<D>(self, deserializer: D) -> Result<bool, D::Error>
    where
        D: Deserializer<'de>,
    {
        Deserialize::deserialize(deserializer)
    }
}

/// Wrapper that adapts the `bool` visitor to the `Option<bool>` field.
pub fn deserialize_verbose<'de, D>(deserializer: D) -> Result<Option<bool>, D::Error>
where
    D: Deserializer<'de>,
{
    // `deserialize_any` hands the raw JSON token to the visitor.
    // If the token is `null` the visitor returns an error, which we map
    // to `Ok(None)`. Any successful conversion becomes `Ok(Some(value))`.
    match deserializer.deserialize_any(VerboseVisitor) {
        Ok(v) => Ok(Some(v)),
        Err(_) => Ok(None), // covers `null` or completely missing key
    }
}
