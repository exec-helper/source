#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <string>
#include <vector>
#include <memory>

namespace execHelper {
    namespace core {
        class TargetDescription;
        class CompilerDescription;
        class AnalyzeDescription;
        class ExecutorInterface;
    }

    namespace config {
        struct SettingsNode;
    }
}

namespace execHelper {
    namespace core {
        typedef std::string Command;
        typedef std::vector<Command> CommandCollection;

        class Options {
            public:
                virtual bool parse(int argc, const char* const * argv) = 0;
                virtual bool parseSettingsFile(const std::string& file) noexcept = 0;

                virtual bool getVerbosity() const noexcept = 0;
                virtual bool getSingleThreaded() const noexcept = 0;
                virtual const CommandCollection& getCommands() const noexcept = 0;
                virtual const TargetDescription& getTarget() const noexcept = 0;
                virtual const CompilerDescription& getCompiler() const noexcept = 0;
                virtual const AnalyzeDescription& getAnalyzeMethods() const noexcept = 0;
                virtual const config::SettingsNode& getSettings() const noexcept = 0;
                virtual config::SettingsNode& getSettings(const std::string& key) noexcept = 0;
                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept = 0;
                virtual bool containsHelp() const noexcept = 0;
                virtual std::shared_ptr<Options> clone() const noexcept = 0;

                virtual void setExecutor(ExecutorInterface* const executor) noexcept = 0;
                virtual ExecutorInterface* getExecutor() const noexcept = 0;

            protected:
                Options() {}
        };
    }
}

#endif  /* __OPTIONS_H__ */
