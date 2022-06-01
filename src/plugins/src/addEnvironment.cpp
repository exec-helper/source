#include "addEnvironment.h"

#include "config/environment.h"
#include "logger.h"

using execHelper::config::SettingsKey;

namespace execHelper::plugins {
void AddEnvironment::getVariables(
    config::VariablesMap& variables,
    const config::FleetingOptionsInterface& /*options*/) noexcept {
    if(!variables.add(SettingsKey(config::ENVIRONMENT_KEY),
                      config::EnvArgs())) {
        LOG(error) << "Failed to add key '" << config::ENVIRONMENT_KEY << "'";
    }
}
} // namespace execHelper::plugins
