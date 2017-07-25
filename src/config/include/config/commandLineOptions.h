#ifndef COMMAND_LINE_OPTIONS_INCLUDE
#define COMMAND_LINE_OPTIONS_INCLUDE

#include <string>
#include <vector>

namespace execHelper {
   namespace config {
        const std::string HELP_KEY{"help"};
        using HelpOption_t = bool;

        const std::string VERBOSE_KEY{"verbose"};
        using VerboseOption_t = bool;

        const std::string DRY_RUN_KEY{"dry-run"};
        using DryRunOption_t = bool;

        const std::string JOBS_KEY{"jobs"};
        using JobsOption_t = std::string;   // Must be string, since the 'auto' keyword is also supported
        using Jobs_t = uint32_t;

        const std::string SETTINGS_FILE_KEY{"settings-file"};
        using SettingsFileOption_t= std::string;

        const std::string COMMAND_KEY{"command"};
        using Command = std::string;
        using CommandCollection = std::vector<Command>;

        const std::string LOG_LEVEL_KEY("debug");
        using LogLevelOption_t = std::string;
   } // config
} // execHelper

#endif  /* COMMAND_LINE_OPTIONS_INCLUDE */
