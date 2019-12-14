#include "commandLine.h"

#include "logger.h"

namespace execHelper::plugins {
void CommandLine::getVariables(
    config::VariablesMap& variables,
    const config::FleetingOptionsInterface& /*options*/) noexcept {
    if(!variables.add(COMMAND_LINE_KEY, CommandLineArgs())) {
        LOG(warning) << "Failed to add key '" << COMMAND_LINE_KEY << "'";
    }
}
} // namespace execHelper::plugins
