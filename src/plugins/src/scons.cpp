#include "scons.h"

#include <string>

#include "config/settingsNode.h"
#include "core/targetDescription.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::TargetDescription;
using execHelper::core::CompilerDescriptionElement;
using execHelper::core::Patterns;
using execHelper::config::SettingsNode;

namespace {
    const string SCONS_COMMAND("scons");
    const string patternsKey("command-line");
}

namespace execHelper { namespace plugins {
    bool Scons::apply(const std::string& command, Task& task, const Options& options) const noexcept {
        if(command == "build") {
            return build(task, options);
        } else if(command == "clean") {
            return clean(task, options);
        }
        return false;
    }

    TaskCollection Scons::getCommandLine(const SettingsNode& settings, const CompilerDescriptionElement& compiler) noexcept {
        static const string commandLineKey("command-line");
        if(! settings.contains(commandLineKey)) {
            return TaskCollection();
        }

        TaskCollection commandArguments = settings[commandLineKey].toStringCollection();
        Patterns patterns = settings["patterns"].toStringCollection();
        for(auto& argument : commandArguments) {
            argument = replacePatterns(argument, patterns, compiler);
        }
        return commandArguments;
    }

    TaskCollection Scons::getMultiThreaded(const SettingsNode& settings) noexcept {
        static const string singleThreadedKey("single-threaded");
        if(settings.contains(singleThreadedKey)) {
            TaskCollection commandArguments = settings[singleThreadedKey].toStringCollection();
            if(commandArguments.size() > 0 && commandArguments[0] == "yes") {
                return TaskCollection();
            }
        }
        return TaskCollection({"-j8"});
    }

    bool Scons::build(core::Task& task, const core::Options& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"scons"});  
        task.append(SCONS_COMMAND);
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(settings));
                newTask.append(getCommandLine(settings, compiler));
                if(target.getTarget() != "all") {
                    string buildTarget = target.getTarget() + target.getRunTarget();
                    newTask.append(buildTarget);
                }
                registerTask(newTask, options);
            }
        }
        return true;
    }

    bool Scons::clean(core::Task& task, const core::Options& options) const noexcept {
        task.append(SCONS_COMMAND);
        task.append("clean");
        const SettingsNode& settings = options.getSettings({"scons"});  
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(settings));
                newTask.append(getCommandLine(settings, compiler));
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
