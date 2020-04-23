#include "verbosity.h"

#include "logger.h"

namespace execHelper::plugins {
void VerbosityLong::getVariables(
    config::VariablesMap& variables,
    const config::FleetingOptionsInterface& options) noexcept {
    const auto* const verbosity = options.getVerbosity() ? "yes" : "no";
    if(!variables.add(VERBOSITY_KEY, verbosity)) {
        LOG(error) << "Failed to add key '" << VERBOSITY_KEY << "'";
    }
}
} // namespace execHelper::plugins
