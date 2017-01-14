#ifndef __PLUGIN_UTILS_H__
#define __PLUGIN_UTILS_H__

#include <string>

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
        const config::SettingsNode& getContainingSettings(const std::string& command, const config::SettingsNode& rootSettings, const std::string& key) noexcept;
        core::TaskCollection getCommandLine(const core::Command& command, const config::SettingsNode& settings, const core::PatternCombinations& patternCombinations = core::PatternCombinations()) noexcept;

        void replacePatternCombinations(core::TaskCollection& commandArguments, const core::PatternCombinations& patternCombinations) noexcept;
    }
}

#endif  /* __PLUGIN_UTILS_H__ */
