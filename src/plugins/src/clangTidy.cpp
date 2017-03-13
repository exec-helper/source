#include "clangTidy.h"

#include "log/log.h"

#include "configValue.h"
#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Options;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::TaskCollection;

namespace {
    const string cppcheckCommand("clang-tidy");
}


namespace execHelper { namespace plugins {
    bool ClangTidy::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static string CLANG_TIDY_KEY("clang-tidy");
        if(!options.getSettings().contains(CLANG_TIDY_KEY)) {
            user_feedback_error("Could not find the '" << CLANG_TIDY_KEY << "' key in the configuration");
            return false;
        }
        const SettingsNode& rootSettings = options.getSettings(CLANG_TIDY_KEY);  
        task.append(cppcheckCommand);

        TaskCollection sources = ConfigValue<TaskCollection>::get("sources", {}, command, rootSettings);

        if(sources.empty()) {
            user_feedback("Warning: no sources defined in the clang-tidy configuration");
        }

        for(const auto& source : sources) {
            Task sourceTask = task;

            const ConfigValue<TaskCollection>::OrderedConfigKeys configKeys = {
                                                    {command, "sources", source}, 
                                                    {command},
                                                    {"sources", source},
                                                    {}
                                                };

            TaskCollection checksCollection = ConfigValue<TaskCollection>::get("checks", {}, rootSettings, configKeys);
            if(!checksCollection.empty()) {
                sourceTask.append(getChecks(checksCollection));
            }

            TaskCollection commandLine = ConfigValue<TaskCollection>::get("command-line", {}, rootSettings, configKeys);
            sourceTask.append(commandLine);
            sourceTask.append(source);

            for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
                Task clangTidyTask = replacePatternCombinations(sourceTask, combination);
                registerTask(clangTidyTask, options);
            }
        }
        return true;
    }

    string ClangTidy::getChecks(const TaskCollection& checksCollection) noexcept {
        if(checksCollection.empty()) {
            return "";
        }
        string result("-checks=");
        for(size_t i = 0; i < checksCollection.size() - 1; ++i) {
            result += checksCollection[i] + ",";
        }
        result += checksCollection.back();
        return result;
    }
} }
