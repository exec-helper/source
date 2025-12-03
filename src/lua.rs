use anyhow::{Result, bail};
use mlua::{UserData, UserDataMethods, Value as LuaValue};
use serde::Deserialize;
use serde_json::{Map, Value as JsonValue, from_value};
use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::{Arc, Mutex};
use tracing::{error, trace};

use mlua::prelude::*;

use crate::common::{
    CommandLineArgument, Environment, ExecutionCommand, PatternReferences, deserialize_arguments,
};
use crate::user_error;

#[derive(Clone, Debug, Deserialize)]
#[serde(rename_all = "kebab-case")]
struct LuaCommand {
    patterns: Option<PatternReferences>,
    environment: Option<Environment>,
    working_dir: Option<String>,
    command_line: Option<Vec<CommandLineArgument>>,

    #[serde(flatten)]
    commands: Option<HashMap<String, LuaCommand>>,
}

struct LuaConfig {
    root: Map<String, JsonValue>,
    command: Map<String, JsonValue>,
}

impl UserData for LuaConfig {
    fn add_methods<M: UserDataMethods<Self>>(methods: &mut M) {
        methods.add_meta_method(
            "__index",
            |lua, this, key: LuaString| -> mlua::Result<Option<LuaValue>> {
                let subtree = this
                    .command
                    .get(&key.to_string_lossy())
                    .or(this.root.get(&key.to_string_lossy()));

                match subtree {
                    Some(tree) => Ok(Some(json_to_lua(lua, tree)?)),
                    None => Ok(None),
                }
            },
        );
    }
}

impl UserData for ExecutionCommand {
    fn add_methods<M: UserDataMethods<Self>>(methods: &mut M) {
        methods.add_method_mut("add_args", |_, this, args: mlua::Value| {
            let mut arguments: Vec<String> = match args {
                mlua::Value::Table(values) => values
                    .sequence_values::<String>()
                    .map(|value| value.unwrap())
                    .collect(),
                _ => {
                    return Err(mlua::Error::FromLuaConversionError {
                        from: args.type_name(),
                        to: "Vec<String>".to_string(),
                        message: Some("expected an array of strings".into()),
                    });
                }
            };

            this.command.append(&mut arguments);
            Ok(())
        });
    }
}

impl FromLua for ExecutionCommand {
    fn from_lua(value: LuaValue, _lua: &Lua) -> mlua::Result<Self> {
        let execution_command = match value {
            mlua::Value::UserData(ud) => ud.take()?,
            other => {
                return Err(mlua::Error::FromLuaConversionError {
                    from: other.type_name(),
                    to: "ExecutionCommand".to_string(),
                    message: Some("expected a table".into()),
                });
            }
        };

        Ok(execution_command)
    }
}

fn json_to_lua(lua: &Lua, json: &JsonValue) -> LuaResult<LuaValue> {
    match json {
        JsonValue::Null => Ok(LuaValue::Nil),
        JsonValue::Bool(b) => Ok(LuaValue::Boolean(*b)),
        JsonValue::Number(n) => {
            if let Some(i) = n.as_i64() {
                Ok(LuaValue::Integer(i))
            } else if let Some(u) = n.as_u64() {
                // Lua integers are signed, so we fall back to float for large unsigned values
                Ok(LuaValue::Number(u as f64))
            } else if let Some(f) = n.as_f64() {
                Ok(LuaValue::Number(f))
            } else {
                // Should never happen, but keep the API happy
                Ok(LuaValue::Nil)
            }
        }
        JsonValue::String(s) => Ok(LuaValue::String(lua.create_string(s)?)),

        // Containers – we allocate a fresh table for each level
        JsonValue::Array(arr) => {
            let tbl = lua.create_table()?;
            for (idx, elem) in arr.iter().enumerate() {
                // Lua arrays are 1‑based
                let lua_elem = json_to_lua(lua, elem)?;
                tbl.set((idx + 1) as i64, lua_elem)?;
            }
            Ok(LuaValue::Table(tbl))
        }
        JsonValue::Object(map) => {
            let tbl = lua.create_table()?;
            for (k, v) in map.iter() {
                let lua_val = json_to_lua(lua, v)?;
                tbl.set(k.as_str(), lua_val)?;
            }
            Ok(LuaValue::Table(tbl))
        }
    }
}

pub fn run_lua_plugin(
    command: &String,
    plugin_config: JsonValue,
    plugin_path: PathBuf,
) -> Result<Vec<ExecutionCommand>> {
    trace!(
        "Generating for command '{}' using plugin @ '{}'",
        command,
        plugin_path.display()
    );

    let lua = Lua::new();

    let root_json = match plugin_config {
        JsonValue::Object(ref map) => map.clone(),
        _ => bail!("The config for {command} should be a map!"),
    };

    let command_json = match root_json.get(command) {
        Some(tree) => match tree {
            JsonValue::Object(map) => map.clone(),
            _ => bail!("The config for {command} should be a map!"),
        },
        None => Map::new(),
    };

    let lua_config = LuaConfig {
        root: root_json,
        command: command_json,
    };
    lua.globals().set("config", lua_config)?;

    let config: LuaCommand = match from_value(plugin_config) {
        Ok(config) => config,
        Err(e) => bail!("Failed to parse lua plugin at '{}'", e.path()),
    };
    trace!("Selected config = {:?}", config);

    let command_config = match config.commands {
        Some(ref commands) => match commands.get(command) {
            Some(config) => config.clone(),
            None => config.clone(),
        },
        None => config.clone(),
    };

    let environment = command_config.environment.or(config.environment);
    let patterns = command_config.patterns.or(config.patterns);
    let working_directory = command_config
        .working_dir
        .or(config.working_dir)
        .unwrap_or(".".to_string());

    let execution_command = ExecutionCommand {
        command: Vec::new(),
        environment,
        patterns,
        working_directory,
    };

    lua.globals().set(
        "one",
        lua.create_function(
            |lua, args: LuaValue| -> mlua::Result<Option<mlua::String>> {
                let result: Option<LuaString> = match args {
                    mlua::Value::Nil => None,
                    mlua::Value::Boolean(b) => match b {
                        true => Some(lua.create_string("true")?),
                        false => Some(lua.create_string("false")?),
                    },
                    //mlua::Value::Integer(i) => Some(i),
                    //mlua::Value::Number(n) => Some(n),
                    mlua::Value::String(s) => Some(s),
                    mlua::Value::Table(t) => t.get(0).ok(),
                    _ => None,
                };
                Ok(result)
            },
        )?,
    )?;

    lua.globals().set(
        "input_error",
        lua.create_function(|_, error: String| -> mlua::Result<String> {
            error!(error);
            user_error!("{}", error);
            Err(mlua::Error::RuntimeError(error))
        })?,
    )?;

    let command_line = command_config.command_line.or(config.command_line).clone();
    lua.globals().set(
        "get_commandline",
        lua.create_function(move |_, ()| match &command_line {
            Some(args) => match deserialize_arguments(args) {
                Ok(args) => Ok(args),
                Err(e) => Err(mlua::Error::RuntimeError(format!(
                    "Failed to deserialize the configured command line: {e}!"
                ))),
            },
            None => Ok(Vec::new()),
        })?,
    )?;

    lua.globals()
        .set("task", lua.create_userdata(execution_command)?)?;

    let commands: Arc<Mutex<Vec<ExecutionCommand>>> = Arc::new(Mutex::new(vec![]));
    let cmds = commands.clone();
    lua.globals().set(
        "register_task",
        lua.create_function(move |_, task: ExecutionCommand| {
            let mut guard = cmds.lock().unwrap();
            guard.push(task);
            Ok(())
        })?,
    )?;

    lua.load(plugin_path).exec()?;
    let mut guard = commands.lock().unwrap();

    Ok(std::mem::take(&mut *guard))
}
