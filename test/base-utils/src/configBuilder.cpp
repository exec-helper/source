#include "configBuilder.h"

using std::move;

using gsl::not_null;

namespace execHelper {
namespace test {
namespace baseUtils {
ConfigBuilder::ConfigBuilder() {
    ExecutionContent::registerIoService(
        gsl::not_null<IoService*>(&m_ioService));
}

Commands::const_iterator ConfigBuilder::begin() const noexcept {
    return m_commands.begin();
}

Commands::const_iterator ConfigBuilder::end() const noexcept {
    return m_commands.end();
}

void ConfigBuilder::add(const TestCommand& command) noexcept {
    m_commands.push_back(command);
}

void ConfigBuilder::add(TestCommand&& command) noexcept {
    m_commands.emplace_back(move(command));
}

void ConfigBuilder::write(not_null<YamlWriter*> yaml) noexcept {
    for(auto& command : m_commands) {
        command.write(yaml);
    }
}

ConfigBuilder::ConfigBuilderRAII ConfigBuilder::startIteration() noexcept {
    m_ioService.run();
    return ConfigBuilderRAII(m_commands);
}

ConfigBuilder::ConfigBuilderRAII::ConfigBuilderRAII(Commands& commands) noexcept
    : m_commands(commands) {
    ;
}

ConfigBuilder::ConfigBuilderRAII::~ConfigBuilderRAII() {
    for(auto& command : m_commands) {
        command.resetExecutions();
    }
};

} // namespace baseUtils
} // namespace test
} // namespace execHelper
