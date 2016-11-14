#ifndef __EXEC_HELPER_OPTIONS_H__
#define __EXEC_HELPER_OPTIONS_H__

#include <string>
#include <vector>

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> CommandCollection;
        typedef std::vector<std::string> TargetCollection;

        class ExecHelperOptions {
            public:
                ExecHelperOptions();
                bool parse(int argc, char** argv);

                bool getVerbosity() const;
                const CommandCollection& getCommands() const;
                const TargetCollection& getTargets() const;

            private:
                bool m_verbose;
                CommandCollection m_commands;
                TargetCollection m_targets;
        };
    }
}

#endif /* __EXEC_HELPER_OPTIONS_H__ */
