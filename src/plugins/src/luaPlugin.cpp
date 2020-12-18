#include "luaPlugin.h"
#include "log/log.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include <LuaContext.hpp>
#ifdef MSVC
#include <misc/exception.hpp>
#endif
#include <boost/optional.hpp>

#include "config/commandLineOptions.h"
#include "core/task.h"

#include "addEnvironment.h"
#include "executePlugin.h"
#include "logger.h"
#include "pluginUtils.h"
#include "workingDirectory.h"

using std::ifstream;
using std::make_pair;
using std::move;
using std::pair;
using std::rethrow_if_nested;
using std::runtime_error;
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;

using boost::optional;

using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::JOBS_KEY;
using execHelper::config::Jobs_t;
using execHelper::config::Path;
using execHelper::config::VariablesMap;
using execHelper::config::VERBOSE_KEY;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Tasks;

namespace filesystem = std::filesystem;

namespace {
class Config {
  public:
    explicit Config(const VariablesMap& config) noexcept : m_config(config) {}

    auto get(const string& key) noexcept
        -> optional<unordered_map<string, string>> {
        if(!m_config.contains(key)) {
            return boost::none;
        }
        const auto& subnode = m_config[key];

        std::unordered_map<std::string, std::string> result;
        if(!subnode.values()) {
            LOG(debug) << key << " has no values";
            return result;
        }

        auto values = *(subnode.values());
        if(values.empty()) {
            LOG(debug) << key << " is empty";
            return result;
        }

        const auto& subsubnode = subnode[(*subnode.values()).front()];
        if(subsubnode.values()) {
            // Construct a map
            for(const auto& value : values) {
                result[value] = *(subnode.get<std::string>(value));
            }
        } else {
            // Construct an array with indices as keys
            for(size_t i = 0; i < values.size(); ++i) {
                result[to_string(i)] = values[i];
            }
        }
        return result;
    }

  private:
    const VariablesMap& m_config;
};
} // namespace

namespace execHelper::plugins {
LuaPlugin::LuaPlugin(Path script) noexcept : m_script(std::move(script)) { ; }

auto LuaPlugin::getVariablesMap(
    const FleetingOptionsInterface& /*fleetingOptions*/) const noexcept
    -> VariablesMap {
    return VariablesMap("luaPlugin");
}

auto LuaPlugin::apply(Task task, const VariablesMap& config) const -> Tasks {
    Tasks tasks;
    LuaContext lua;

    WorkingDirectory::apply(task, config);
    AddEnvironment::apply(task, config);

    try {
        lua.writeVariable("verbose", config.get<bool>(VERBOSE_KEY, false));
        lua.writeVariable("jobs", config.get<Jobs_t>(JOBS_KEY, 1U));

        lua.executeCode("function get_commandline() "
                        "return list(config['command-line']) or {} "
                        "end");

        lua.executeCode("function get_environment() "
                        "return config['environment'] or {} "
                        "end");

        lua.executeCode("function get_verbose(verbose_command)"
                        "if one(config['verbose']) "
                        "then "
                        "if one(config['verbose']) == 'yes' "
                        "then "
                        "return {verbose_command} "
                        "end "
                        "else "
                        "if verbose "
                        "then "
                        "return {verbose_command} "
                        "end "
                        "end "
                        "return {} "
                        "end");

        // Define the Config class
        Config configWrapper(config);

        lua.writeVariable("config", configWrapper);

        lua.writeFunction<optional<unordered_map<string, string>>(
            Config&, const std::string&)>(
            "config", LuaContext::Metatable, "__index",
            [](Config& config, const std::string& key) {
                return config.get(key);
            });

        // Define the Task class
        lua.writeVariable("task", task);
        lua.registerFunction<Task, Task()>(
            "new", [](const Task& /*task*/) { return Task(); });
        lua.registerFunction<Task, Task()>(
            "copy", [](const Task& task) { return Task(task); });
        lua.registerFunction<Task, void(const vector<pair<int, string>>&)>(
            "add_args", [](Task& task, const vector<pair<int, string>>& args) {
                std::for_each(
                    args.begin(), args.end(),
                    [&task](const auto& arg) { task.append(arg.second); });
            });

        lua.writeFunction("register_task", [&tasks](Task task) {
            tasks.emplace_back(move(task));
        });

        lua.writeFunction("register_tasks",
                          [&tasks](const vector<pair<int, Task>>& newTasks) {
                              transform(
                                  newTasks.begin(), newTasks.end(),
                                  back_inserter(tasks),
                                  [](const auto& task) { return task.second; });
                          });

        lua.writeFunction<Tasks(const Task&, const vector<pair<int, string>>&)>(
            "run_target",
            [](const Task& task,
               const vector<pair<int, string>>& commands) -> Tasks {
                Tasks tasks;
                for(const auto& combination :
                    makePatternPermutator(task.getPatterns())) {
                    CommandCollection commandsToExecute;
                    commandsToExecute.reserve(commands.size());

                    transform(commands.begin(), commands.end(),
                              back_inserter(commandsToExecute),
                              [&combination](const auto& command) {
                                  return replacePatternCombinations(
                                      command.second, combination);
                              });

                    ExecutePlugin executePlugin(commandsToExecute);

                    core::Task newTask =
                        replacePatternCombinations(task, combination);
                    try {
                        auto newTasks = executePlugin.apply(
                            move(newTask), VariablesMap("subtask"));
                        move(newTasks.begin(), newTasks.end(),
                             back_inserter(tasks));
                    } catch(const std::runtime_error& e) {
                        user_feedback_error(e.what());
                        rethrow_if_nested(e);
                    }
                }
                return tasks;
            });

        lua.writeFunction<optional<string>(
            const optional<unordered_map<string, string>>&)>(
            "one",
            [](const optional<unordered_map<string, string>>& values)
                -> optional<string> {
                if(!values) {
                    return boost::none;
                }
                return (*values).at("0");
            });

        lua.writeFunction("input_error", [](const string& message) {
            throw std::runtime_error(message);
        });

        lua.writeFunction("user_feedback", [](const string& message) {
            user_feedback_error(message);
        });
        lua.writeFunction("user_feedback_error", [](const string& message) {
            user_feedback_error(message);
        });
        lua.writeFunction(
            "list",
            [](const optional<unordered_map<string, string>>& values)
                -> optional<std::vector<pair<int, string>>> {
                if(!values) {
                    return boost::none;
                }

                vector<pair<int, string>> result;
                result.reserve(values->size());

                auto index = 1;

                for(size_t i = 0U; i < values->size(); ++i) {
                    auto listValue =
                        make_pair(index, (*values).at(std::to_string(i)));
                    result.push_back(listValue);
                    ++index;
                }
                return result;
            });

        lua.writeFunction<bool(const std::string&)>(
            "isdir", [](const std::string& path) {
                filesystem::path ppath(path);
                return filesystem::is_directory(ppath);
            });
    } catch(std::exception& e) {
        LOG(error) << "Internal error: '" << e.what() << "'";
        throw runtime_error(
            string("Lua plugin: Internal error: ").append(e.what()));
    }

    try {
        ifstream executionCode(m_script);
        lua.executeCode(executionCode);
    } catch(const LuaContext::SyntaxErrorException& e) {
        LOG(error) << "Syntax error detected in lua file '" +
                          m_script.string() + "': " + e.what();
        throw runtime_error(string("Syntax error detected in lua file '")
                                .append(m_script.string())
                                .append("': ")
                                .append(e.what()));
    } catch(const LuaContext::ExecutionErrorException& e) {
        LOG(error) << e.what();
        throw runtime_error(e.what());
    } catch(const std::runtime_error& e) {
        LOG(error) << e.what();
        rethrow_if_nested(e);
    }
    return tasks;
}

auto LuaPlugin::summary() const noexcept -> std::string {
    return "Lua plugin for module " + m_script.string();
}
} // namespace execHelper::plugins
