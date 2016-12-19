#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>
#include <memory>

#include "options.h"

#include "config/settingsNode.h"

#include "targetDescription.h"
#include "compilerDescription.h"
#include "analyzeDescription.h"

namespace execHelper {
    namespace core {
        class ExecHelperOptions : public Options {
            public:
                ExecHelperOptions() noexcept;
                ExecHelperOptions(const ExecHelperOptions& other) noexcept;

                std::string getSettingsFile(int argc, const char* const * argv) const noexcept;
                virtual bool parse(int argc, const char* const * argv) override;
                virtual bool parseSettingsFile(const std::string& file) noexcept override;

                virtual bool getVerbosity() const noexcept override;
                virtual bool getSingleThreaded() const noexcept override;
                virtual const CommandCollection& getCommands() const noexcept override;
                virtual const TargetDescription& getTarget() const noexcept override;
                virtual const CompilerDescription& getCompiler() const noexcept override;
                virtual const AnalyzeDescription& getAnalyzeMethods() const noexcept override;
                virtual const config::SettingsNode& getSettings() const noexcept override;
                virtual config::SettingsNode& getSettings(const std::string& key) noexcept override;
                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept override;
                virtual bool containsHelp() const noexcept override;
                virtual std::shared_ptr<Options> clone() const noexcept override;

                void printHelp() const noexcept;

                virtual void setExecutor(ExecutorInterface* const executor) noexcept override;
                virtual ExecutorInterface* getExecutor() const noexcept override;

            private:
                bool m_help;
                bool m_verbose;
                bool m_singleThreaded;
                CommandCollection m_commands;
                std::unique_ptr<TargetDescription> m_target;
                std::unique_ptr<CompilerDescription> m_compiler;
                std::unique_ptr<AnalyzeDescription> m_analyze;
                config::SettingsNode m_settings;

                ExecutorInterface* m_executor;       // Non-owning pointer
        };
    }
}

#endif /* __EXEC_HELPER_OPTIONS_H__ */
