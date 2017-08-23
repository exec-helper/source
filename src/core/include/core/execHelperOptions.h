#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "options.h"

#include "config/settingsNode.h"

#include "optionDescriptions.h"
#include "patternsHandler.h"

namespace execHelper {
    namespace core {
        class OptionDescriptions;
    }
}

namespace execHelper {
    namespace core {
        class ExecHelperOptions : public Options {
            public:
                ExecHelperOptions() noexcept;
                ExecHelperOptions(const ExecHelperOptions& other) = delete;
                ExecHelperOptions(ExecHelperOptions&& other) noexcept = delete;
                ~ExecHelperOptions() override = default;

                ExecHelperOptions& operator=(const ExecHelperOptions& other) = delete;
                ExecHelperOptions& operator=(ExecHelperOptions&& other) = delete;

                bool operator==(const ExecHelperOptions& other) const noexcept;
                bool operator!=(const ExecHelperOptions& other) const noexcept;

                void swap(ExecHelperOptions& other) noexcept;

                std::string getSettingsFile(int argc, const char* const * argv) const noexcept;
                bool parse(int argc, const char* const * argv) override;
                bool parseSettingsFile(const config::Path& file) noexcept override;

                bool getVerbosity() const noexcept override;
                bool getDryRun() const noexcept override;
                bool getSingleThreaded() const noexcept override;
                boost::optional<log::LogLevel> getLogLevel() const noexcept override;
                const CommandCollection& getCommands() const noexcept override;
                const config::SettingsNode& getSettings() const noexcept override;
                const config::SettingsNode& getSettings(const std::string& key) noexcept override;
                const config::SettingsNode& getSettings(const std::string& key) const noexcept override;
                bool containsHelp() const noexcept override;
                bool contains(const std::string& longOptions) const noexcept override;
                std::vector<std::string> getLongOption(const std::string& longOptions) const noexcept override;
                const PatternsHandler& getPatternsHandler() const noexcept override;
                PatternValues getValues(const Pattern& pattern) const noexcept override;
                PatternPermutator makePatternPermutator(const PatternKeys& patterns) const noexcept override;

                void printHelp() const noexcept;

                void setExecutor(ExecutorInterface* executor) noexcept override;
                ExecutorInterface* getExecutor() const noexcept override;

            private:
                bool m_verbose{false};
                bool m_dryRun{false};
                bool m_singleThreaded{false};
                CommandCollection m_commands;
                config::SettingsNode m_settings;
                boost::program_options::variables_map m_optionsMap;
                PatternsHandler m_patternsHandler;
                execHelper::core::OptionDescriptions m_optionsDescriptions;

                ExecutorInterface* m_executor{nullptr};       // Non-owning pointer
        };
    }
}

#endif /* __EXEC_HELPER_OPTIONS_H__ */
