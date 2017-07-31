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
        class SettingsNode;
    }
}

namespace execHelper {
    namespace core {
        typedef std::string Command;
        typedef std::vector<Command> CommandCollection;

        typedef std::map<PatternKey, PatternValue> PatternCombinations;
        typedef MapPermutator<PatternKey, PatternValue> PatternPermutator;

        /**
         * \brief Interface for classes that interpret command line options
         */
        class Options {
            public:
                virtual ~Options() {};

                /**
                 * Parses the given options
                 *
                 * \param argc     The number of options that were passed on the command line
                 * \param argv     An array of pointers the beginning of each argument as a char string
                 * \returns True    If the options could be successfully parsed
                 *          False   Otherwise
                 */
                virtual bool parse(int argc, const char* const * argv) = 0;

                /**
                 * Parses the given settings file
                 *
                 * \param file     The settings file to parse
                 * \returns True    If the settings file was successfully parsed
                 *          False   Otherwise
                 */
                virtual bool parseSettingsFile(const std::string& file) noexcept = 0;

                /**
                 * Returns whether verbose mode is set
                 *
                 * \returns True    If verbose mode is set
                 *          False   Otherwise
                 */
                virtual bool getVerbosity() const noexcept = 0;

                /**
                 * Returns whether dry run mode is set
                 *
                 * \returns True    If dry run mode is set
                 *          False   Otherwise
                 */
                virtual bool getDryRun() const noexcept = 0;

                /**
                 * Returns whether single threaded mode is set
                 *
                 * \returns True    If single threaded mode is set
                 *          False   Otherwise
                 */
                virtual bool getSingleThreaded() const noexcept = 0;

                /**
                 * Returns the commands that were set on the command line
                 *
                 * \returns A collection of commands given on the command line
                 */
                virtual const CommandCollection& getCommands() const noexcept = 0;

                /**
                 * Returns the root settings node
                 *
                 * \returns The root settings node
                 */
                virtual const config::SettingsNode& getSettings() const noexcept = 0;

                /**
                 * Returns the settings node associated with the given key
                 *
                 * \param key  The associated key to return the settings node for
                 * \returns The settings node associated with the given key
                 */
                virtual const config::SettingsNode& getSettings(const std::string& key) noexcept = 0;

                /*! @copydoc getSettings(const std::string&)
                 */
                virtual const config::SettingsNode& getSettings(const std::string& key) const noexcept = 0;

                /**
                 * Returns whether the help option was set
                 *
                 * \returns True    If the help option was set
                 *          False   Otherwise
                 */
                virtual bool containsHelp() const noexcept = 0;

                /**
                 * Returns whether the pattern settings contain a specific long option
                 *
                 * \param longOptions   The long option to check for
                 * \returns True    If the pattern settings contain the long option
                 *          False   Otherwise
                 */
                virtual bool contains(const std::string& longOptions) const noexcept = 0;

                /**
                 * Get the values set for a specific long option
                 *
                 * \param longOptions  The long option
                 * \returns A collection of the options set on the command line for the specified long option
                 */
                virtual std::vector<std::string> getLongOption(const std::string& longOptions) const noexcept = 0;

                /**
                 * Returns the handler for patterns
                 *
                 * \returns The handler for patterns
                 */
                virtual const PatternsHandler& getPatternsHandler() const noexcept = 0;

                /**
                 * Returns the values associated with the given pattern
                 *
                 * \param pattern  The pattern to get the values for
                 * \returns A collection of the values associated with the pattern
                 */
                virtual PatternValues getValues(const Pattern& pattern) const noexcept = 0;

                /**
                 * Creates a permutator that permutates all options of the given pattern keys
                 *
                 * \param patterns The pattern keys to permutate over
                 * \returns A permutator over the given pattern keys
                 */
                virtual PatternPermutator makePatternPermutator(const PatternKeys& patterns) const noexcept = 0;

                /**
                 * Sets the executor
                 *
                 * \param executor     The executor to set. The caller stays the owner of the executor and should not remove it while it is set for this option object
                 */
                virtual void setExecutor(ExecutorInterface* const executor) noexcept = 0;

                /**
                 * Gets the executor
                 *
                 * \returns     The executor interface to use
                 */
                virtual ExecutorInterface* getExecutor() const noexcept = 0;

            protected:
                Options() {}
        };
    }
}

#endif  /* __OPTIONS_H__ */
