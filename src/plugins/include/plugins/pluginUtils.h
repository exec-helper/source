#ifndef __PLUGIN_UTILS_H__
#define __PLUGIN_UTILS_H__

#include <string>

#include <boost/optional/optional.hpp>

#include "config/settingsNode.h"
#include "core/options.h"
#include "core/task.h"
#include "core/patternsHandler.h"

namespace execHelper {
    namespace core {
        class TargetDescriptionElement;
        class CompilerDescriptionElement;
    }
    namespace plugins {
        class Plugin;
    }
}

namespace execHelper {
    namespace plugins {
        const std::string& getPatternsKey() noexcept;
        const std::string& getCommandLineKey() noexcept;
        const config::SettingsNode& getContainingSettings(const std::string& command, const config::SettingsNode& rootSettings, const std::string& key) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& settings) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& settings, const core::PatternCombinations& patternCombinations) noexcept;

        void replacePatternCombinations(core::TaskCollection& commandArguments, const core::PatternCombinations& patternCombinations) noexcept;
        core::Task replacePatternCombinations(const core::Task& task, const core::PatternCombinations& patternCombinations) noexcept;

        boost::optional<std::string> getConfigurationSetting(const std::string& command, const config::SettingsNode& rootSettings, const std::string& configKey, const std::string& prepend = std::string()) noexcept;
        boost::optional<core::TaskCollection> getConfigurationSettings(const std::string& command, const config::SettingsNode& rootSettings, const std::string& configKey) noexcept;
        core::PatternPermutator makePatternPermutator(const core::Command& command, const config::SettingsNode& rootSettings, const core::Options& options) noexcept;
    }
}

#endif  /* __PLUGIN_UTILS_H__ */
