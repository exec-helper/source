#include "buildPlugin.h"

#include <string>

using std::string;

namespace execHelper::plugins {
const std::string& BuildPlugin::getBuildDirKey() noexcept {
    static const string buildDirKey("build-dir");
    return buildDirKey;
}
} // namespace execHelper::plugins
