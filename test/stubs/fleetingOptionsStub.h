#ifndef FLEETING_OPTIONS_STUB_INCLUDE
#define FLEETING_OPTIONS_STUB_INCLUDE

#include "config/fleetingOptionsInterface.h"

namespace execHelper {
namespace test {
class FleetingOptionsStub : public config::FleetingOptionsInterface {
  public:
    virtual ~FleetingOptionsStub() = default;

    config::HelpOption_t getHelp() const noexcept override { return m_help; }

    config::VersionOption_t getVersion() const noexcept override {
        return m_version;
    }

    config::VerboseOption_t getVerbosity() const noexcept override {
        return m_verbose;
    }

    config::DryRunOption_t getDryRun() const noexcept override {
        return m_dryRun;
    }

    config::Jobs_t getJobs() const noexcept override { return m_jobs; }

    auto listPlugins() const noexcept -> config::ListPluginsOption_t override {
        return m_listPlugins;
    }

    const config::CommandCollection& getCommands() const noexcept override {
        return m_commands;
    }

    config::HelpOption_t m_help = {false};
    config::VersionOption_t m_version = {false};
    config::VerboseOption_t m_verbose = {false};
    config::DryRunOption_t m_dryRun = {false};
    config::Jobs_t m_jobs = 1024U;
    config::ListPluginsOption_t m_listPlugins = {false};
    config::CommandCollection m_commands = {};
};
} // namespace test

} // namespace execHelper

#endif /* FLEETING_OPTIONS_STUB_INCLUDE */
