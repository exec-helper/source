#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "pattern.h"
#include "mapPermutator.h"

namespace execHelper {
    namespace core {
        class ExecutorInterface;
        class PatternsHandler;
    }

    namespace config {
        struct SettingsNode;
    }
}

namespace execHelper {
    namespace core {
        typedef std::string Command;
        typedef std::vector<Command> CommandCollection;

        typedef std::map<PatternKey, PatternValue> PatternCombinations;
        typedef MapPermutator<PatternKey, PatternValue> PatternPermutator;

        class Options {
            public:
                virtual ~Options() {};

                virtual bool parse(int argc, const char* const * argv) = 0;
                virtual bool parseSettingsFile(const std::string& file) noexcept = 0;

                virtual bool getVerbosity() const noexcept = 0;
                virtual bool getDryRun() const noexcept = 0;
                virtual bool getSingleThreaded() const noexcept = 0;
                virtual const CommandCollection& getCommands() const noexcept = 0;
                virtual const config::SettingsNode& getSettings() const noexcept = 0;
                virtual const config::SettingsNode& getSettings(const std::string& key) noexcept = 0;
                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept = 0;
                virtual bool containsHelp() const noexcept = 0;
                virtual bool contains(const std::string& longOptions) const noexcept = 0;
                virtual std::vector<std::string> getLongOption(const std::string& longOptions) const noexcept = 0;
                virtual const PatternsHandler& getPatternsHandler() const noexcept = 0;
                virtual PatternValues getValues(const Pattern& pattern) const noexcept = 0;
                virtual PatternPermutator makePatternPermutator(const PatternKeys& patterns) const noexcept = 0;

                virtual void setExecutor(ExecutorInterface* const executor) noexcept = 0;
                virtual ExecutorInterface* getExecutor() const noexcept = 0;

            protected:
                Options() {}
        };
    }
}

#endif  /* __OPTIONS_H__ */
