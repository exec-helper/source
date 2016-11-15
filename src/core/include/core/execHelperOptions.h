#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>
#include <memory>

#include "target.h"

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> CommandCollection;

        class ExecHelperOptions {
            public:
                ExecHelperOptions();
                bool parse(int argc, char** argv);

                bool getVerbosity() const noexcept;
                const CommandCollection& getCommands() const noexcept;
                const TargetDescription& getTarget() const noexcept;

            private:
                bool m_verbose;
                CommandCollection m_commands;
                std::shared_ptr<TargetDescription> m_target;
        };
    }
}

#endif /* __EXEC_HELPER_OPTIONS_H__ */
