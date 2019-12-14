#include "threadedness.h"

#include "logger.h"

namespace execHelper::plugins {
void JobsLong::getVariables(
    config::VariablesMap& variables,
    const config::FleetingOptionsInterface& options) noexcept {
    if(!variables.add(JOBS_KEY, std::to_string(options.getJobs()))) {
        LOG(error) << "Failed to add key '" << JOBS_KEY << "'";
    }
}
} // namespace execHelper::plugins
