#include "buildPlugin.h"

#include <string>

using std::string;

namespace execHelper::plugins {
auto BuildPlugin::getBuildDirKey() noexcept -> const std::string& {
    static const string buildDirKey("build-dir");
    return buildDirKey;
}
} // namespace execHelper::plugins
