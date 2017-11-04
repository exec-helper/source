#ifndef FLEETING_OPTIONS_STUB_INCLUDE
#define FLEETING_OPTIONS_STUB_INCLUDE

#include "config/fleetingOptionsInterface.h"

namespace execHelper {
namespace test {
class FleetingOptionsStub : public config::FleetingOptionsInterface {
  public:
    virtual ~FleetingOptionsStub() = default;

    config::HelpOption_t getHelp() const noexcept override { return m_help; }

    config::VerboseOption_t getVerbosity() const noexcept override {
        return m_verbose;
    }

    config::DryRunOption_t getDryRun() const noexcept override {
        return m_dryRun;
    }

    config::Jobs_t getJobs() const noexcept override { return m_jobs; }

    const config::CommandCollection& getCommands() const noexcept override {
        return m_commands;
    }

    config::HelpOption_t m_help = {false};
    config::VerboseOption_t m_verbose = {false};
    config::DryRunOption_t m_dryRun = {false};
    config::Jobs_t m_jobs = 1024U;
    config::CommandCollection m_commands = {};
};
} // namespace test

} // namespace execHelper

#endif /* FLEETING_OPTIONS_STUB_INCLUDE */
