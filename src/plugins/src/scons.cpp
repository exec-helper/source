#include "scons.h"

#include <string>

#include "config/settingsNode.h"
#include "core/targetDescription.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

using std::string;
using execHelper::core::Command;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::TargetDescription;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;
using execHelper::config::SettingsNode;

namespace {
    const string SCONS_COMMAND("scons");
}

namespace execHelper { namespace plugins {
    bool Scons::apply(const Command& command, Task& task, const Options& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"scons"});

        task.append(SCONS_COMMAND);
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                if(getMultiThreaded(command, settings, options)) {
                    newTask.append(TaskCollection({"--jobs", "8"}));
                }
                newTask.append(getCommandLine(command, settings, compiler));
                if(target.getTarget() != "all") {
                    string buildTarget = target.getTarget() + target.getRunTarget();
                    newTask.append(buildTarget);
                }
                registerTask(newTask, options);
            }
        }
        return true;
    }
} }
