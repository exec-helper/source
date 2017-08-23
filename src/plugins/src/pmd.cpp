#include "pmd.h"

#include <string>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/task.h"

#include "configValue.h"
#include "logger.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;

namespace execHelper { namespace plugins {
    bool Pmd::apply(const std::string& command, Task task, const Options& options) const noexcept {
        const string pmdConfigKey("pmd");
        const SettingsNode& rootSettings = options.getSettings(pmdConfigKey);

        static string execConfigKey("exec");
        boost::optional<string> execCommand = ConfigValue<string>::getSetting(execConfigKey, rootSettings, command);
        if(execCommand == boost::none) {
            user_feedback_error("Could not find the '" << execConfigKey << "' setting for command '" << command << "' in the '" << pmdConfigKey << "' settings");
            return false;
        }
        task.append(execCommand.get());

        static const string toolConfigKey("tool");
        boost::optional<string> tool = ConfigValue<string>::getSetting(toolConfigKey, rootSettings, command);
        if(tool == boost::none) {
            user_feedback_error("Could not find the '" << toolConfigKey << "' setting for command '" << command << "' in the '" << pmdConfigKey << "' settings");
            return false;
        }

        task.append(tool.get());
        if(tool.get() == "cpd") {
            static const string minimumTokensConfigKey("minimum-tokens");
            boost::optional<string> minimumTokens = ConfigValue<string>::getSetting(minimumTokensConfigKey, rootSettings, command);
            if(minimumTokens != boost::none) {
                task.append({"--minimum-tokens", minimumTokens.get()});
            }

            static const string filesConfigKey("files");
            boost::optional<TaskCollection> files = ConfigValue<TaskCollection>::getSetting(filesConfigKey, rootSettings, command);
            if(files != boost::none) {
                for(const auto& file : files.get()) {
                    task.append({"--files", file});
                }
            }
        }

        static const string languageConfigKey("language");
        boost::optional<string> language = ConfigValue<string>::getSetting(languageConfigKey, rootSettings, command);
        if(language != boost::none) {
            task.append({"--language", language.get()});
        }

        if(options.getVerbosity()) {
            task.append("-verbose");
        }

        task.append(ConfigValue<TaskCollection>::get(getCommandLineKey(), {}, command, rootSettings));

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task makeTask = replacePatternCombinations(task, combination);
            if(! registerTask(makeTask, options)) {
                return false;
            }
        }
        return true;
    }
} // namespace plugins
} // namespace execHelper
