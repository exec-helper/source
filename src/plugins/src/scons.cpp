#include "scons.h"

#include <map>
#include <string>

#include "config/settingsNode.h"
#include "core/patterns.h"
#include "core/patternsHandler.h"

#include "configValue.h"
#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::PatternKeys;

namespace execHelper { namespace plugins {
    bool Scons::apply(const Command& command, Task task, const Options& options) const noexcept {
        static const string SCONS_COMMAND("scons");

        const SettingsNode& rootSettings = options.getSettings({"scons"});

        task.append(SCONS_COMMAND);

        if(getMultiThreaded(command, rootSettings, options)) {
            task.append(TaskCollection({"--jobs", "8"}));
        }
        if(options.getVerbosity()) {
            task.append("--debug=explain");
        }
        task.append(ConfigValue<TaskCollection>::get(getCommandLineKey(), {}, command, rootSettings));

        for(const auto& combination : makePatternPermutator(command, rootSettings, options)) {
            Task sconsTask = replacePatternCombinations(task, combination);
            registerTask(sconsTask, options);
        }
        return true;
    }
} }
