#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>
#include <memory>

#include "targetDescription.h"
#include "compilerDescription.h"

#include "config/settingsNode.h"

namespace execHelper {
    namespace core {
        class ExecutorInterface;
    }
}

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> CommandCollection;

        class ExecHelperOptions {
            public:
                ExecHelperOptions();
                std::string getSettingsFile(int argc, char** argv) const noexcept;
                bool parse(int argc, char** argv);
                bool parseSettingsFile(const std::string& file) noexcept;

                bool getVerbosity() const noexcept;
                const CommandCollection& getCommands() const noexcept;
                const TargetDescription& getTarget() const noexcept;
                const CompilerDescription& getCompiler() const noexcept;
                const config::SettingsNode& getSettings(const std::string& key) const noexcept;

                void setExecutor(ExecutorInterface* const executor) noexcept;
                ExecutorInterface* getExecutor() const noexcept;

            private:
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
