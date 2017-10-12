#ifndef PLUGINS_INCLUDE
#define PLUGINS_INCLUDE

#include <string>

namespace execHelper {
    namespace test {
        namespace baseUtils {
            struct CommandPluginSettings {
                static inline std::string getKey() {
                    return COMMAND_KEY;
                }
                
                static inline std::string getStatementKey() {
                    assert(false);  // The command plugin does not have a command line key
                    return COMMAND_KEY;
                }           
            };

            struct CommandLineCommandSettings {
                static inline std::string getKey() {
                    return COMMAND_LINE_COMMAND_KEY;
                }
                
                static inline std::string getStatementKey() {
                    return COMMAND_LINE_COMMAND_LINE_KEY;
                }
            };
        } // namespace baseUtils
    } // namespace test
} // namespace execHelper

#endif  /* PLUGINS_INCLUDE */
