#include "execHelperOptions.h"

#include <iostream>
#include <string>

#include <boost/program_options.hpp>

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

namespace {
    options_description getOptionDescriptions() {
        options_description descriptions("Allowed options");
        descriptions.add_options()
            ("help,h", "Produce help message")
            ("verbose,v", "Set verbosity")
            ("command,c", value<execHelper::CommandCollection>()->multitoken(), "Set commands")
            ("target,t", value<execHelper::TargetCollection>()->multitoken(), "Set targets")
        ;
        return descriptions;
    }
}

namespace execHelper {
    ExecHelperOptions::ExecHelperOptions() :
        m_verbose(false),
        m_targets{"all"}
    {
        ;
    }

    bool ExecHelperOptions::getVerbosity() const {
        return m_verbose;
    }

    const execHelper::CommandCollection& ExecHelperOptions::getCommands() const {
        return m_commands;
    }

    const execHelper::TargetCollection& ExecHelperOptions::getTargets() const {
        return m_targets;
    }

    bool ExecHelperOptions::parse(int argc, char** argv) {
        const options_description optionDescriptions = getOptionDescriptions();

        // Assign positional arguments
        positional_options_description positionalOptionsDesc; 
        positionalOptionsDesc.add("command", -1);
       
        variables_map optionsMap;
        store(command_line_parser(argc, argv).options(optionDescriptions).positional(positionalOptionsDesc).run(), optionsMap);
        notify(optionsMap);    

        if(optionsMap.count("help")) {
            cout << optionDescriptions << endl;
            return true;
        }

        if(optionsMap.count("verbose")) {
            m_verbose = true;
        }

        if(optionsMap.count("command")) {
            m_commands.clear();
            m_commands = optionsMap["command"].as<CommandCollection>();
        }

        if(optionsMap.count("target")) {
            m_targets.clear();
            m_targets = optionsMap["target"].as<TargetCollection>();
        }
        return true;
    }
}
