#include "optionDescriptions.h"

using std::string;

using boost::program_options::options_description;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::notify;
using boost::program_options::command_line_parser;
using boost::program_options::value;
using boost::program_options::positional_options_description;

namespace execHelper { namespace core {
    OptionDescriptions::OptionDescriptions() noexcept :
        m_descriptions(m_optionDescription.add_options()) 
    {
        // Add the default options
        m_descriptions("help,h", "Produce help message")
            ("verbose,v", "Set verbosity")
            ("command,z", value<CommandCollection>()->multitoken(), "Set commands")
            ("settings-file,s", value<string>(), "Set settings file")
            ("single-threaded,u", "Set single threaded")
            ("dry-run,n", "Dry run exec-helper")
        ;
    }

    options_description OptionDescriptions::getOptionDescriptions() const noexcept {
        return m_optionDescription;
    }

    variables_map OptionDescriptions::getOptionsMap(int argc, const char* const* argv, bool allowUnregistered) const noexcept {
        // Assign positional arguments
        positional_options_description positionalOptionsDesc;
        positionalOptionsDesc.add("command", -1);

        variables_map optionsMap;
        if(! allowUnregistered) {
            store(command_line_parser(argc, argv).options(m_optionDescription).positional(positionalOptionsDesc).run(), optionsMap);
        } else {
            store(command_line_parser(argc, argv).options(m_optionDescription).positional(positionalOptionsDesc).options(m_optionDescription).allow_unregistered().run(), optionsMap);
        }
        notify(optionsMap);
        return optionsMap;
    }
} }
