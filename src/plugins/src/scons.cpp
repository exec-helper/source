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
    const string patternsKey("patterns");
}

namespace execHelper { namespace plugins {
    bool Scons::apply(const Command& command, Task& task, const Options& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"scons"});

        task.append(SCONS_COMMAND);
        for(const auto& compiler : options.getCompiler()) {
            for(const auto& target : options.getTarget()) {
                Task newTask = task;
                newTask.append(getMultiThreaded(command, settings));
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

    TaskCollection Scons::getCommandLine(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& compiler) noexcept {
        static const string commandLineKey("command-line");
        const SettingsNode settings = getContainingSettings(command, rootSettings, commandLineKey); 
        if(! settings.contains(commandLineKey)) {
            return TaskCollection();
        }

        TaskCollection commandArguments = settings[commandLineKey].toStringCollection();
        const SettingsNode patternSettings = getContainingSettings(command, rootSettings, patternsKey); 
        Patterns patterns = patternSettings[patternsKey].toStringCollection();
        for(auto& argument : commandArguments) {
            argument = replacePatterns(argument, patterns, compiler);
        }
        return commandArguments;
    }

    const SettingsNode& Scons::getContainingSettings(const Command& command, const SettingsNode& rootSettings, const string& key) noexcept {
        if(rootSettings.contains(command) && rootSettings[command].contains(key)) {
            return rootSettings[command]; 
        }
        return rootSettings;
    }

    TaskCollection Scons::getMultiThreaded(const std::string& command, const SettingsNode& rootSettings) noexcept {
        static const string singleThreadedKey("single-threaded");
        const SettingsNode settings = getContainingSettings(command, rootSettings, singleThreadedKey); 
        if(settings.contains(singleThreadedKey)) {
            TaskCollection commandArguments = settings[singleThreadedKey].toStringCollection();
            if(commandArguments.size() > 0 && commandArguments[0] == "yes") {
                return TaskCollection();
            }
        }
        return TaskCollection({"-j8"});
    }

    //bool Scons::build(core::Task& task, const core::Options& options) const noexcept {
        //const SettingsNode& settings = options.getSettings({"scons"});  
        //task.append(SCONS_COMMAND);
        //for(const auto& compiler : options.getCompiler()) {
            //for(const auto& target : options.getTarget()) {
                //Task newTask = task;
                //newTask.append(getMultiThreaded(settings));
                //newTask.append(getCommandLine(settings, compiler));
                //if(target.getTarget() != "all") {
                    //string buildTarget = target.getTarget() + target.getRunTarget();
                    //newTask.append(buildTarget);
                //}
                //registerTask(newTask, options);
            //}
        //}
        //return true;
    //}

    //bool Scons::clean(core::Task& task, const core::Options& options) const noexcept {
        //task.append(SCONS_COMMAND);
        //task.append("clean");
        //const SettingsNode& settings = options.getSettings({"scons"});  
        //for(const auto& compiler : options.getCompiler()) {
            //for(const auto& target : options.getTarget()) {
                //Task newTask = task;
                //newTask.append(getMultiThreaded(settings));
                //newTask.append(getCommandLine(settings, compiler));
                //if(target.getTarget() != "all") {
                    //string buildTarget = target.getTarget() + target.getRunTarget();
                    //newTask.append(buildTarget);
                //}
                //registerTask(newTask, options);
            //}
        //}
        //return true;
    //}
} }
