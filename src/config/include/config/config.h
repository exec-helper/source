#ifndef CONFIG_INCLUDE
#define CONFIG_INCLUDE

#include <string>
#include <utility>

#include <boost/optional/optional.hpp>

#include "path.h"
#include "pattern.h"
#include "settingsNode.h"

namespace execHelper {
    namespace config {
        class OptionDescriptions;
    }
}

namespace execHelper {
    namespace config {
        using PatternSettingsPair = std::pair<Patterns, config::SettingsNode>;
                
        boost::optional<PatternSettingsPair> parseSettingsFile(const Path& file) noexcept;
    } // config
} // execHelper

#endif  /* CONFIG_INCLUDE */
