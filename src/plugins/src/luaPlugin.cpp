#include "luaPlugin.h"

#include <algorithm>
#include <iterator>
#include <optional>
#include <unordered_map>
#include <utility>

#include <integral/integral.hpp>

#include "config/pattern.h"
#include "core/task.h"

#include "addEnvironment.h"
#include "executePlugin.h"
#include "logger.h"
#include "pluginUtils.h"
#include "threadedness.h"
#include "verbosity.h"
#include "workingDirectory.h"

using std::optional;
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;

using execHelper::config::CommandCollection;
using execHelper::config::FleetingOptionsInterface;
using execHelper::config::Path;
using execHelper::config::PatternKey;
using execHelper::config::Patterns;
using execHelper::config::PatternValues;
using execHelper::config::VariablesMap;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Tasks;

namespace {
class Config {
  public:
    explicit Config(const VariablesMap& config) noexcept : m_config(config) {}

    auto get(const std::string& key) noexcept
        -> optional<unordered_map<std::string, std::string>> {
        if(!m_config.contains(key)) {
            LOG(debug) << "key '" << key << "' does not exist in config"
                       << std::endl;
            return std::nullopt;
        }
        auto& subnode = m_config[key];

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

        auto& subsubnode = subnode[(*subnode.values()).front()];
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

class PatternWrapper {
  public:
    explicit PatternWrapper(const Patterns& patterns) : m_patterns(patterns) {}

    auto get(const string& key) noexcept -> optional<vector<string>> {
        auto found = find_if(
            m_patterns.begin(), m_patterns.end(),
            [&key](const auto& pattern) { return key == pattern.getKey(); });
        if(found == m_patterns.end()) {
            // Key not found
            return std::nullopt;
        }
        return found->getValues();
    }

  private:
    const Patterns& m_patterns;
};

} // namespace

namespace execHelper::plugins {
LuaPlugin::LuaPlugin(Path script) noexcept : m_script(std::move(script)) { ; }

auto LuaPlugin::getVariablesMap(const FleetingOptionsInterface& fleetingOptions)
    const noexcept -> VariablesMap {
    VariablesMap defaults{"luaPlugin"};

    VerbosityLong::getVariables(defaults, fleetingOptions);
    JobsLong::getVariables(defaults, fleetingOptions);
    return defaults;
}

auto LuaPlugin::apply(Task task, const VariablesMap& config,
                      const Patterns& patterns) const noexcept -> bool {
    Tasks tasks;
    integral::State state;

    WorkingDirectory::apply(task, config);
    AddEnvironment::apply(task, config);

    try {
        state.openLibs();

        state["verbose"] = config.get<Verbosity>(VERBOSITY_KEY, false);
        state["jobs"] = config.get<Jobs>(JOBS_KEY, 1U);

        // Add custom modules to lua's package.path
        state.doString(
            "package.path = package.path .. "
            "';/home/verhagenconsultancy/workspace/exec-helper/branches/"
            "study-embedding-lue/lua/?.lua'");

        state.doString("function get_commandline() "
                       "return list(config['command-line']) or {} "
                       "end");

        state.doString("function get_environment() "
                       "return config['environment'] or {} "
                       "end");
        state.doString("function get_verbose(verbose_command)"
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

        // Add custom modules to lua's package.path
        Config configWrapper(config);

        state["Config"] = integral::ClassMetatable<Config>().setFunction(
            "__index", &Config::get);

        state["config"] = configWrapper;

        // Define the Task class
        state["Task"] =
            integral::ClassMetatable<Task>()
                .setConstructor<Task()>("new")
                .setConstructor<Task(const Task&)>("copy")
                .setFunction("add_args",
                             [](Task& task, const TaskCollection& args) {
                                 task.append(args);
                             });

        // Set the initial task
        state["task"] = task;

        // Define the Pattern class
        //state["Pattern"] = integral::ClassMetatable<Pattern>().setFunction("get_values", &Pattern::getValues);
        state["Patterns"] =
            integral::ClassMetatable<PatternWrapper>().setFunction(
                "__index", &PatternWrapper::get);

        PatternWrapper patternWrapper(patterns);
        state["patterns"] = patternWrapper;

        state["register_task"].setFunction(
            [&tasks](const Task& task) { tasks.push_back(task); });

        state["run_target"].setFunction(
            [](const Task& task, const CommandCollection& commandsToExecute) {
                ExecutePlugin executePlugin(commandsToExecute);
                return executePlugin.apply(task, VariablesMap("subtask"),
                                           Patterns());
            });

        state["one"].setFunction(
            [](const optional<std::unordered_map<std::string, std::string>>&
                   values) -> optional<std::string> {
                if(!values) {
                    return std::nullopt;
                }
                return (*values).at("0");
            });

        state["user_feedback"].setFunction(
            [](const std::string& message) { user_feedback_error(message); });

        state["user_feedback_error"].setFunction(
            [](const std::string& message) { user_feedback_error(message); });

        state["list"].setFunction(
            [](const optional<std::unordered_map<std::string, std::string>>&
                   values) -> optional<std::vector<std::string>> {
                if(!values) {
                    return std::nullopt;
                }
                std::vector<std::string> result;
                result.reserve(values->size());

                for(size_t i = 0U; i < values->size(); ++i) {
                    result.push_back((*values).at(std::to_string(i)));
                }
                return result;
            });

    } catch(std::exception& e) {
        user_feedback_error("Internal error");
        LOG(error) << "Internal error: '" << e.what() << "'";
        return false;
    }

    try {
        state.doFile(m_script);
    } catch(integral::StateException& e) {
        user_feedback_error("Failed to apply lua file '" + m_script.string() +
                            "'");
        LOG(error) << "Failed to apply lua file '" << m_script.string()
                   << "': " << e.what();
        return false;
    }

    for(const auto& task : tasks) {
        for(const auto& combination : makePatternPermutator(patterns)) {
            core::Task newTask = replacePatternCombinations(task, combination);
            if(!registerTask(newTask)) {
                return false;
            }
        }
    }
    return true;
}
} // namespace execHelper::plugins
