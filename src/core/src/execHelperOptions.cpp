#include "execHelperOptions.h"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "log/log.h"
#include "yaml/yaml.h"
#include "config/settingsNode.h"
#include "compiler.h"
#include "mode.h"
#include "executorInterface.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

using boost::program_options::options_description;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::notify;
using boost::program_options::command_line_parser;
using boost::program_options::value;
using boost::program_options::positional_options_description;

using execHelper::core::CommandCollection;
using execHelper::core::TargetDescription;
using execHelper::core::CompilerDescription;

using execHelper::config::SettingsNode;

using execHelper::yaml::Yaml;
using execHelper::yaml::YamlFile;

namespace {
    options_description getOptionDescriptions() {
        options_description descriptions("Allowed options");
        descriptions.add_options()
            ("help,h", "Produce help message")
            ("verbose,v", "Set verbosity")
            ("command,z", value<CommandCollection>()->multitoken(), "Set commands")
            ("target,t", value<TargetDescription::TargetCollection>()->multitoken(), "Set targets")
            ("run-target,r", value<TargetDescription::RunTargetCollection>()->multitoken(), "Set run targets")
            ("compiler,c", value<CompilerDescription::CompilerNames>()->multitoken(), "Set compilers")
            ("mode,m", value<CompilerDescription::ModeNames>()->multitoken(), "Set modes")
            ("architecture,a", value<CompilerDescription::ArchitectureNames>()->multitoken(), "Set architecture")
            ("settings-file,s", value<string>(), "Set settings file")
        ;
        return descriptions;
    }

    variables_map getOptionsMap(int argc, char** argv) {
        const options_description optionDescriptions = getOptionDescriptions();

        // Assign positional arguments
        positional_options_description positionalOptionsDesc; 
        positionalOptionsDesc.add("command", -1);
       
        variables_map optionsMap;
        store(command_line_parser(argc, argv).options(optionDescriptions).positional(positionalOptionsDesc).run(), optionsMap);
        notify(optionsMap);    
        return optionsMap;
    }
    
}

namespace execHelper { namespace core {
    ExecutorInterface* ExecHelperOptions::m_executor(0);

    ExecHelperOptions::ExecHelperOptions() :
        m_help(false),
        m_verbose(false),
        m_compiler(new CompilerDescription({Compiler("gcc"), Compiler("clang")}, {Mode("debug"), Mode("release")}, {Architecture("x64")}))
    {
        if(! m_executor) {

        }
    }

    bool ExecHelperOptions::getVerbosity() const noexcept {
        return m_verbose;
    }

    const CommandCollection& ExecHelperOptions::getCommands() const noexcept {
        return m_commands;
    }

    const CompilerDescription& ExecHelperOptions::getCompiler() const noexcept {
        assert(m_compiler);
        return *m_compiler;
    }

    const TargetDescription& ExecHelperOptions::getTarget() const noexcept {
        assert(m_target);
        return *m_target;
    }

    string ExecHelperOptions::getSettingsFile(int argc, char** argv) const noexcept {
        variables_map optionsMap = getOptionsMap(argc, argv);
        if(optionsMap.count("settings-file")) {
            return optionsMap["settings-file"].as<string>();
        }
        return ".exec-helper";
    }

    bool ExecHelperOptions::parse(int argc, char** argv) {
        variables_map optionsMap = getOptionsMap(argc, argv);
        if(optionsMap.count("help")) {
            m_help = true;
            return true;
        }

        if(optionsMap.count("verbose")) {
            m_verbose = true;
        }

        if(optionsMap.count("command")) {
            m_commands.clear();
            m_commands = optionsMap["command"].as<CommandCollection>();
        }

        TargetDescription::TargetCollection targets = {"all"};
        if(optionsMap.count("target")) {
            targets = optionsMap["target"].as<TargetDescription::TargetCollection>();
        }

        TargetDescription::RunTargetCollection runTargets = {"all"};
        if(optionsMap.count("run-target")) {
            runTargets = optionsMap["run-target"].as<TargetDescription::RunTargetCollection>();
        }
        m_target.reset(new TargetDescription(targets, runTargets));

        if(optionsMap.count("compiler")) {
            CompilerDescription::CompilerCollection compilers = CompilerDescription::convertToCompilerCollection(optionsMap["compiler"].as<CompilerDescription::CompilerNames>());
            m_compiler.reset(new CompilerDescription(compilers, m_compiler->getModes(), m_compiler->getArchitectures()));
        }

        if(optionsMap.count("mode")) {
            CompilerDescription::ModeCollection modes = CompilerDescription::convertToModeCollection(optionsMap["mode"].as<CompilerDescription::ModeNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), modes, m_compiler->getArchitectures()));
        }

        if(optionsMap.count("architecture")) {
            CompilerDescription::ArchitectureCollection architectures = CompilerDescription::convertToArchitectureCollection(optionsMap["architecture"].as<CompilerDescription::ArchitectureNames>());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), architectures));
        }

        return true;
    }

    bool ExecHelperOptions::parseSettingsFile(const std::string& file) noexcept {
        YamlFile yamlFile;
        yamlFile.file = file;
        Yaml yaml(yamlFile);
        if(! yaml.getTree({}, m_settings)) {
            LOG("Could not get settings tree");
            return false;
        }

        if(m_settings.contains("default-compilers")) {
            CompilerDescription::CompilerCollection compilers = CompilerDescription::convertToCompilerCollection(m_settings["default-compilers"].toStringCollection());
            m_compiler.reset(new CompilerDescription(compilers, m_compiler->getModes(), m_compiler->getArchitectures()));
        }

        if(m_settings.contains("default-modes")) {
            CompilerDescription::ModeCollection modes = CompilerDescription::convertToModeCollection(m_settings["default-modes"].toStringCollection());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), modes, m_compiler->getArchitectures()));
        }

        if(m_settings.contains("default-architectures")) {
            CompilerDescription::ArchitectureCollection architectures = CompilerDescription::convertToArchitectureCollection(m_settings["default-architectures"].toStringCollection());
            m_compiler.reset(new CompilerDescription(m_compiler->getCompilers(), m_compiler->getModes(), architectures));
        }

        return true;
    }

    bool ExecHelperOptions::containsHelp() const noexcept {
        return m_help;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) const noexcept {
       return m_settings[key]; 
    }

    void ExecHelperOptions::setExecutor(ExecutorInterface* const executor) noexcept {
        m_executor = executor;
    }

    ExecutorInterface* ExecHelperOptions::getExecutor() const noexcept {
        return m_executor;
    }

    void ExecHelperOptions::printHelp() const noexcept {
        user_feedback(getOptionDescriptions());
    }
} }
