#include "buildDir.h"

#include "buildPlugin.h"
#include "logger.h"

namespace execHelper::plugins {
void BuildDirLong::getVariables(
    config::VariablesMap& variables,
    const config::FleetingOptionsInterface& /*options*/) noexcept {
    if(!variables.add(BuildPlugin::getBuildDirKey(), ".")) {
        LOG(error) << "Failed to add key '" << BuildPlugin::getBuildDirKey()
                   << "'";
    }
}
} // namespace execHelper::plugins
