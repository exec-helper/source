#ifndef HANDLERS_INCLUDE
#define HANDLERS_INCLUDE

#include <algorithm>
#include <filesystem>

#include "config/environment.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "plugins/commandLine.h"

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
} // namespace execHelper::plugins::test

#endif /* HANDLERS_INCLUDE */
