#ifndef HANDLERS_INCLUDE
#define HANDLERS_INCLUDE

#include <algorithm>
#include <filesystem>

#include "config/environment.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/commandLine.h"

#include "plugins/plugin.h"
#include "unittest/catch.h"

namespace execHelper::plugins::test {
inline void handleEnvironment(const config::EnvironmentCollection& environment,
                              config::SettingsNode& config,
                              core::Task& expectedTask) noexcept {
    std::for_each(
        environment.begin(), environment.end(), [&config](const auto& env) {
            REQUIRE(config.add({"environment", env.first}, env.second));
        });
    expectedTask.setEnvironment(environment);
}

inline void handleCommandLine(const CommandLineArgs& commandLine,
                              config::SettingsNode& config,
                              core::Task& expectedTask) noexcept {
    REQUIRE(config.add("command-line", commandLine));
    expectedTask.append(commandLine);
}

inline void handleWorkingDirectory(const std::filesystem::path& workingDir,
                                   config::SettingsNode& config,
                                   core::Task& expectedTask) noexcept {
    REQUIRE(config.add("working-dir", workingDir));
    expectedTask.setWorkingDirectory(workingDir);
}

inline void handleVerbosity(bool verbose, const std::string& flag,
                            config::SettingsNode& config,
                            core::Task& expectedTask) noexcept {
    if(verbose) {
        REQUIRE(config.add("verbose", "yes"));
        expectedTask.append(flag);
    } else {
        REQUIRE(config.add("verbose", "no"));
    }
}

constexpr auto memoryFunction() {
    return
        [](core::Task task,
           [[maybe_unused]] const config::VariablesMap& variablesMap,
           [[maybe_unused]] const plugins::ExecutionContext& context) noexcept
        -> core::Tasks { return {std::move(task)}; };
}

/**
 * Register the given values as a separate command that is associated with its own memory backend
 *
 * \param[in] values    The values to create a command for
 * \returns A collection of values mapped to plugin functions
 */
[[nodiscard]] inline auto
mapToMemories(const std::vector<std::string>& values) noexcept {
    Plugins plugins;

    std::transform(values.begin(), values.end(),
                   std::inserter(plugins, plugins.end()),
                   [](const auto& value) {
                       return std::make_pair(value, memoryFunction());
                   });
    return plugins;
}
} // namespace execHelper::plugins::test

#endif /* HANDLERS_INCLUDE */
