#ifndef __PLUGIN_UTILS_H__
#define __PLUGIN_UTILS_H__

#include <string>

#include <boost/optional/optional.hpp>

#include "config/path.h"
#include "config/settingsNode.h"
#include "core/options.h"
#include "core/patternsHandler.h"
#include "core/task.h"

namespace execHelper {
    namespace core {
        class TargetDescriptionElement;
        class CompilerDescriptionElement;
    } // namespace core
    namespace plugins {
        class Plugin;
    } // namespace plugins
} // namespace execHelper

namespace execHelper {
    namespace plugins {
        const std::string& getPatternsKey() noexcept;
        const std::string& getCommandLineKey() noexcept;
        const std::string& getEnvironmentKey() noexcept;
        const std::string& getWorkingDirKey() noexcept;

        boost::optional<const config::SettingsNode&> getContainingSettings(const std::string& key, const config::SettingsNode& rootSettings, const std::vector<std::string>& configKeys) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& rootSettings, const core::PatternCombinations& patternCombinations) noexcept;
        core::EnvironmentCollection getEnvironment(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;
        boost::optional<config::Path> getWorkingDir(const core::Command& command, const config::SettingsNode& rootSettings) noexcept;

        core::Task replacePatternCombinations(const core::Task& task, const core::PatternCombinations& patternCombinations) noexcept;

        boost::optional<std::string> getConfigurationSetting(const std::string& command, const config::SettingsNode& rootSettings, const std::string& configKey, const std::string& prepend = std::string()) noexcept;
        boost::optional<core::TaskCollection> getConfigurationSettings(const std::string& command, const config::SettingsNode& rootSettings, const std::string& configKey) noexcept;
        core::PatternPermutator makePatternPermutator(const core::Command& command, const config::SettingsNode& rootSettings, const core::Options& options) noexcept;
    } // namespace plugins
} // namespace execHelper

#endif  /* __PLUGIN_UTILS_H__ */
