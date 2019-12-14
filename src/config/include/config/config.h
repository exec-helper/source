#ifndef CONFIG_INCLUDE
#define CONFIG_INCLUDE

#include <optional>
#include <string>
#include <utility>

#include "path.h"
#include "pattern.h"
#include "settingsNode.h"

namespace execHelper {
namespace config {
class OptionDescriptions;
}
} // namespace execHelper

namespace execHelper {
namespace config {
using PatternSettingsPair = std::pair<Patterns, config::SettingsNode>;

auto parseSettingsFile(const Path& file) noexcept
    -> std::optional<PatternSettingsPair>;
} // namespace config
} // namespace execHelper

#endif /* CONFIG_INCLUDE */
