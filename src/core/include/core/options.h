#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <string>
#include <vector>

namespace execHelper {
    namespace core {
        class TargetDescription;
        class CompilerDescription;
        class ExecutorInterface;
    }

    namespace config {
        struct SettingsNode;
    }
}

namespace execHelper {
    namespace core {
        typedef std::vector<std::string> CommandCollection;

        class Options {
            public:
                virtual bool parse(int argc, char** argv) = 0;
                virtual bool parseSettingsFile(const std::string& file) noexcept = 0;

                virtual bool getVerbosity() const noexcept = 0;
                virtual const CommandCollection& getCommands() const noexcept = 0;
                virtual const TargetDescription& getTarget() const noexcept = 0;
                virtual const CompilerDescription& getCompiler() const noexcept = 0;
                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept = 0;
                virtual bool containsHelp() const noexcept = 0;

                virtual void setExecutor(ExecutorInterface* const executor) noexcept = 0;
                virtual ExecutorInterface* getExecutor() const noexcept = 0;

            protected:
                Options() {}
        };
    }
}

#endif  /* __OPTIONS_H__ */
