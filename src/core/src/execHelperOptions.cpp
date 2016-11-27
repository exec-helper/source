#include "execHelperOptions.h"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "yaml/yaml.h"
#include "config/settingsNode.h"

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

using execHelper::config::SettingsNode;

using execHelper::yaml::Yaml;
using execHelper::yaml::YamlFile;

namespace {
    options_description getOptionDescriptions() {
        options_description descriptions("Allowed options");
        descriptions.add_options()
            ("help,h", "Produce help message")
            ("verbose,v", "Set verbosity")
            ("command,c", value<CommandCollection>()->multitoken(), "Set commands")
            ("target,t", value<TargetDescription::TargetCollection>()->multitoken(), "Set targets")
            ("run-target,r", value<TargetDescription::RunTargetCollection>()->multitoken(), "Set run targets")
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
    ExecHelperOptions::ExecHelperOptions() :
        m_verbose(false)
    {
        ;
    }

    bool ExecHelperOptions::getVerbosity() const noexcept {
        return m_verbose;
    }

    const execHelper::core::CommandCollection& ExecHelperOptions::getCommands() const noexcept {
        return m_commands;
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
            cout << getOptionDescriptions() << endl;
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
        return true;
    }

    bool ExecHelperOptions::parseSettingsFile(const std::string& file) noexcept {
        YamlFile yamlFile;
        yamlFile.file = file;
        Yaml yaml(yamlFile);
        m_settings = yaml.getTree({}); 
        return true;
    }

    const config::SettingsNode& ExecHelperOptions::getSettings(const std::string& key) const noexcept {
       return m_settings[key]; 
    }
} }
