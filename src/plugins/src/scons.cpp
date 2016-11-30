#include "scons.h"

#include <string>

#include "config/settingsNode.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::ExecHelperOptions;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescription;
using execHelper::core::Patterns;
using execHelper::config::SettingsNode;

namespace {
    const string SCONS_COMMAND("scons");
}

namespace execHelper { namespace plugins {
    bool Scons::apply(const std::string& command, Task& task, const ExecHelperOptions& options) const noexcept {
        if(command == "build") {
            return build(task, options);
        } else if(command == "clean") {
            return clean(task, options);
        }
        return false;
    }

    //TaskCollection Scons::getBuildDir(const SettingsNode& settings, const CompilerDescription& compiler) noexcept {
        //TaskCollection commandArguments = settings["build-dir"].toStringCollection();
        //Patterns patterns = settings["patterns"].toStringCollection();
        //if(commandArguments.size() == 1U) {
            //return TaskCollection({replacePatterns(commandArguments[0], patterns, compiler)});
        //}
        //return TaskCollection({});
    //}

    TaskCollection Scons::getCommandLine(const SettingsNode& settings, const CompilerDescription& compiler) noexcept {
        TaskCollection commandArguments = settings["command-line"].toStringCollection();
        Patterns patterns = settings["patterns"].toStringCollection();
        for(auto& argument : commandArguments) {
            argument = replacePatterns(argument, patterns, compiler);
        }
        return commandArguments;
    }

    TaskCollection Scons::getMultiThreaded(const SettingsNode& settings) noexcept {
        TaskCollection commandArguments = settings["single-threaded"].toStringCollection();
        if(commandArguments[0] != "yes") {
            return TaskCollection({"-j8"});
        }
        return TaskCollection();
    }

    bool Scons::build(core::Task& task, const core::ExecHelperOptions& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"scons"});  
        task.append(SCONS_COMMAND);
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(settings));
                newTask.append(getCommandLine(settings, compiler));
                string buildTarget = target.getTargets()[0] + target.getRunTargets()[0];
                if(buildTarget != "all") {
                    newTask.append(buildTarget);
                }
                registerTask(newTask, options);
            }
        }
        return true;
    }

    bool Scons::clean(core::Task& task, const core::ExecHelperOptions& options) const noexcept {
        task.append(SCONS_COMMAND);
        task.append("clean");
        const SettingsNode& settings = options.getSettings({"scons"});  
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(settings));
                newTask.append(getCommandLine(settings, compiler));
                string buildTarget = target.getTargets()[0] + target.getRunTargets()[0];
                if(buildTarget != "all") {
                    newTask.append(buildTarget);
                }
                registerTask(newTask, options);
            }
        }
        return true;
    }
} }
