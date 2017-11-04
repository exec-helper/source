#include "buildPlugin.h"

#include <string>

using std::string;

namespace execHelper {
namespace plugins {
const std::string& BuildPlugin::getBuildDirKey() noexcept {
    static const string buildDirKey("build-dir");
    return buildDirKey;
}
} // namespace plugins
} // namespace execHelper
