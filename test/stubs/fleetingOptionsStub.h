#ifndef FLEETING_OPTIONS_STUB_INCLUDE
#define FLEETING_OPTIONS_STUB_INCLUDE

#include "config/fleetingOptionsInterface.h"

namespace execHelper::test {
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

    auto getKeepGoing() const noexcept -> config::KeepGoingOption_t override {
        return m_keepGoing;
    }

    auto getLogLevel() const noexcept -> log::LogLevel override {
        return m_logLevel;
    }

    config::Jobs_t getJobs() const noexcept override { return m_jobs; }

    [[nodiscard]] auto listPlugins() const noexcept
        -> config::ListPluginsOption_t override {
        return m_listPlugins;
    }

    [[nodiscard]] auto appendedSearchPaths() const noexcept
        -> const config::Paths& override {
        return m_appendSearchPaths;
    }

    const config::CommandCollection& getCommands() const noexcept override {
        return m_commands;
    }

    auto getAutoComplete() const noexcept
        -> const std::optional<config::AutoCompleteOption_t>& override {
        return m_autocomplete;
    }

    config::HelpOption_t m_help = {false};
    config::VersionOption_t m_version = {false};
    config::VerboseOption_t m_verbose = {true};
    config::DryRunOption_t m_dryRun = {false};
    config::KeepGoingOption_t m_keepGoing = {false};
    log::LogLevel m_logLevel = {log::warning};
    config::Jobs_t m_jobs = 1024U;
    config::ListPluginsOption_t m_listPlugins = {false};
    config::Paths m_appendSearchPaths = {};
    config::CommandCollection m_commands = {};
    std::optional<config::AutoCompleteOption_t> m_autocomplete;
};
} // namespace execHelper::test

#endif /* FLEETING_OPTIONS_STUB_INCLUDE */
