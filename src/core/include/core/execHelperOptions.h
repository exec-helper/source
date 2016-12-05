#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>
#include <memory>

#include "options.h"

#include "config/settingsNode.h"

#include "targetDescription.h"
#include "compilerDescription.h"

namespace execHelper {
    namespace core {
        class ExecHelperOptions : public Options {
            public:
                ExecHelperOptions();
                std::string getSettingsFile(int argc, char** argv) const noexcept;
                virtual bool parse(int argc, char** argv) override;
                virtual bool parseSettingsFile(const std::string& file) noexcept override;

                virtual bool getVerbosity() const noexcept override;
                virtual const CommandCollection& getCommands() const noexcept override;
                virtual const TargetDescription& getTarget() const noexcept override;
                virtual const CompilerDescription& getCompiler() const noexcept override;
                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept override;
                virtual bool containsHelp() const noexcept override;

                void printHelp() const noexcept;

                virtual void setExecutor(ExecutorInterface* const executor) noexcept override;
                virtual ExecutorInterface* getExecutor() const noexcept override;

            private:
                bool m_help;
                bool m_verbose;
                CommandCollection m_commands;
                std::unique_ptr<TargetDescription> m_target;
                std::unique_ptr<CompilerDescription> m_compiler;
                config::SettingsNode m_settings;

                static ExecutorInterface* m_executor;       // Non-owning pointer
        };
    }
}

#endif /* __EXEC_HELPER_OPTIONS_H__ */
