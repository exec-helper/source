#include "clangStaticAnalyzer.h"

#include <string>
#include <vector>

#include "log/log.h"
#include "config/settingsNode.h"
#include "core/task.h"
#include "core/patterns.h"
#include "core/compilerDescription.h"
#include "core/execHelperOptions.h"

#include "pluginUtils.h"
#include "executePlugin.h"

using std::string;
using std::vector;
using std::shared_ptr;

using execHelper::core::Task;
using execHelper::core::Options;
using execHelper::core::TaskCollection;
using execHelper::core::Patterns;
using execHelper::core::Command;
using execHelper::core::Compiler;
using execHelper::core::Mode;
using execHelper::core::Architecture;
using execHelper::core::Distribution;
using execHelper::core::CompilerDescriptionElement;
using execHelper::config::SettingsNode;
using execHelper::plugins::getPlugin;
using execHelper::plugins::getContainingSettings;

namespace {
    const string clangStaticAnalyzerCommand("scan-build");

    string getCompilerSetting(const Command& command, const SettingsNode& rootSettings, const string& key, const string& defaultValue) {
        string elementName(defaultValue);
        const SettingsNode& elementSettings = getContainingSettings(command, rootSettings, key);
        if(elementSettings.contains(key)) {
            auto collection = elementSettings[key].toStringCollection();
            if(collection.size() > 0) {
                elementName = collection.back();
            }
        }
        return elementName;
    }

    void setOption(const string& optionKey, const string& value, vector<string>& args) noexcept {
        args.push_back(std::string(optionKey));
        args.push_back(value);
    }

}

namespace execHelper { namespace plugins {
    bool ClangStaticAnalyzer::apply(const Command& command, Task& task, const Options& options) const noexcept {
        static string clangStaticAnalyzerKey("clang-static-analyzer");
        const SettingsNode& rootSettings = options.getSettings(clangStaticAnalyzerKey);  
        task.append(clangStaticAnalyzerCommand);
        task.append(::execHelper::plugins::getCommandLine(command, rootSettings));

        CompilerDescriptionElement compiler = getCompiler(command, rootSettings, *options.getCompiler().begin());
        shared_ptr<Options> buildSystemOptions = options.clone();

        string cleanCommand = getSystemName(command, "clean-command", rootSettings);
        string buildCommand = getSystemName(command, "build-command", rootSettings);
        getBuildOptions(*buildSystemOptions, cleanCommand, compiler);
        ExecutePlugin executePlugin({cleanCommand, buildCommand});
        return executePlugin.apply(command, task, *buildSystemOptions);
    }

    CompilerDescriptionElement ClangStaticAnalyzer::getCompiler(const Command& command, const SettingsNode& rootSettings, const CompilerDescriptionElement& defaultElement) noexcept {
        static string compilerKey("compiler");
        static string modeKey("mode");
        static string architectureKey("architecture");
        static string distributionKey("distribution");

        string compilerName = getCompilerSetting(command, rootSettings, compilerKey, defaultElement.getCompiler().getName());
        string modeName = getCompilerSetting(command, rootSettings, modeKey, defaultElement.getMode().getMode());
        string architectureName = getCompilerSetting(command, rootSettings, architectureKey, defaultElement.getArchitecture().getArchitecture());
        string distributionName = getCompilerSetting(command, rootSettings, distributionKey, defaultElement.getDistribution().getDistribution());
        return CompilerDescriptionElement(compilerName, modeName, architectureName, distributionName); 
    }

    string ClangStaticAnalyzer::getSystemName(const Command& command, const string& key, const SettingsNode& rootSettings) noexcept {
        const SettingsNode& settings = getContainingSettings(command, rootSettings, key);
        if(!settings.contains(key)) {
            return ""; 
        }
        TaskCollection systemSetting = settings[key].toStringCollection();
        if(systemSetting.empty()) {
            return ""; 
        }
        return systemSetting.back();
    }

    void ClangStaticAnalyzer::getBuildOptions(Options& options, const Command& /*command*/, const CompilerDescriptionElement& compiler) noexcept {
        static string compilerKey("compiler");
        vector<string> args;
        args.push_back(std::string("clang-static-analyzer"));

        setOption("--compiler", compiler.getCompiler().getName(), args);
        setOption("--mode", compiler.getMode().getMode(), args);
        setOption("--architecture", compiler.getArchitecture().getArchitecture(), args);
        setOption("--distribution", compiler.getDistribution().getDistribution(), args);

        int argc = args.size();
        const char* argv[argc];
        for(int i = 0; i < argc; ++i) {
            argv[i] = args[i].c_str();
        }
        options.parse(argc, argv);
    }
} }
