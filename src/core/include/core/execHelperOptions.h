#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>
#include <memory>

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
                ExecHelperOptions(const ExecHelperOptions& other) noexcept;

                bool operator==(const ExecHelperOptions& other) const noexcept;
                bool operator!=(const ExecHelperOptions& other) const noexcept;

                std::string getSettingsFile(int argc, const char* const * argv) const noexcept;
                virtual bool parse(int argc, const char* const * argv) override;
                virtual bool parseSettingsFile(const config::Path& file) noexcept override;

                virtual bool getVerbosity() const noexcept override;
                virtual bool getDryRun() const noexcept override;
                virtual bool getSingleThreaded() const noexcept override;
                virtual const CommandCollection& getCommands() const noexcept override;
                virtual const config::SettingsNode& getSettings() const noexcept override;
                virtual const config::SettingsNode& getSettings(const std::string& key) noexcept override;
                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept override;
                virtual bool containsHelp() const noexcept override;
                virtual bool contains(const std::string& longOptions) const noexcept override;
                virtual std::vector<std::string> getLongOption(const std::string& longOptions) const noexcept override;
                virtual const PatternsHandler& getPatternsHandler() const noexcept override;
                virtual PatternValues getValues(const Pattern& pattern) const noexcept override;
                virtual PatternPermutator makePatternPermutator(const PatternKeys& patterns) const noexcept override;

                void printHelp() const noexcept;

                virtual void setExecutor(ExecutorInterface* const executor) noexcept override;
                virtual ExecutorInterface* getExecutor() const noexcept override;

            private:
                bool m_verbose;
                bool m_dryRun;
                bool m_singleThreaded;
                CommandCollection m_commands;
                config::SettingsNode m_settings;
                boost::program_options::variables_map m_optionsMap;
                PatternsHandler m_patternsHandler;
                OptionDescriptions m_optionsDescriptions;

                ExecutorInterface* m_executor;       // Non-owning pointer
        };
    }
}

#endif /* __EXEC_HELPER_OPTIONS_H__ */
