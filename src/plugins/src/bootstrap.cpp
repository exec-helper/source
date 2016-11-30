#include "bootstrap.h"

#include <string>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/compilerDescription.h"
#include "core/patterns.h"

using std::string;
using execHelper::core::Task;
using execHelper::core::ExecHelperOptions;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescription;
using execHelper::core::Patterns;
using execHelper::config::SettingsNode;

namespace execHelper { namespace plugins {
    bool Bootstrap::apply(const std::string& command, Task& task, const ExecHelperOptions& options) const noexcept {
        if(command == "init") {
            return init(task, options);
        }
        return false;
    }

    TaskCollection Bootstrap::getBuildDir(const SettingsNode& settings, const CompilerDescription& compiler) noexcept {
        TaskCollection commandArguments = settings["build-dir"].toStringCollection();
        Patterns patterns = settings["patterns"].toStringCollection();
        if(commandArguments.size() == 1U) {
            return TaskCollection({replacePatterns(commandArguments[0], patterns, compiler)});
        }
        return TaskCollection({});
    }

    string Bootstrap::getBootstrapFilename(const SettingsNode& settings) noexcept {
        TaskCollection commandArguments = settings["filename"].toStringCollection();
        if(commandArguments.size() > 0U) {
            return commandArguments[0];
        }
        return "bootstrap.sh";
    }

    bool Bootstrap::init(core::Task& task, const core::ExecHelperOptions& options) const noexcept {
        const SettingsNode& settings = options.getSettings({"bootstrap"});  
        for(const auto& compiler : options.getCompiler()) {
            Task bootstrapTask = task;
            TaskCollection buildTarget = getBuildDir(settings, compiler);
            if(buildTarget.size() > 0U && !buildTarget[0].empty()) {
                bootstrapTask.append("cd");
                bootstrapTask.append(buildTarget);
                bootstrapTask.append("&&");
            }

            string filename = getBootstrapFilename(settings);
            bootstrapTask.append(string("./") + filename);

            registerTask(bootstrapTask, options);
        }
        return true;
    }
} }
