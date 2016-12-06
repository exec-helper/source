#include "make.h"

#include <string>

#include "config/settingsNode.h"
#include "core/task.h"
#include "core/targetDescription.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::Command;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::TargetDescription;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;
using execHelper::config::SettingsNode;

namespace {
    const string MAKE_COMMAND("make");
}

namespace execHelper { namespace plugins {
    bool Make::apply(const Command& command, Task& task, const Options& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"make"});  
        task.append(MAKE_COMMAND);
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(command, settings));
                newTask.append(getCommandLine(command, settings, compiler));
                TaskCollection buildTarget = getBuildDir(command, settings, compiler);
                if(!buildTarget.empty()) {
                    newTask.append("--directory=" + buildTarget[0]);
                }

                string targetName = target.getTarget();
                string runTargetName = target.getRunTarget();
                if(targetName != "all") {
                    newTask.append(targetName + runTargetName);
                }
                registerTask(newTask, options);
            }
        }
        return true;

    }
} }
