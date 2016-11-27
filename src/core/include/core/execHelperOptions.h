#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>
#include <memory>

#include "targetDescription.h"

#include "config/settingsNode.h"

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
                const config::SettingsNode& getSettings(const std::string& key) const noexcept;

            private:
                bool m_verbose;
                CommandCollection m_commands;
                std::unique_ptr<TargetDescription> m_target;
                config::SettingsNode m_settings;
        };
    }
}

#endif /* __EXEC_HELPER_OPTIONS_H__ */
