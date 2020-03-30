#include "optionDescriptions.h"

#include "argv.h"
#include "logger.h"

using std::string;

using boost::program_options::command_line_parser;
using boost::program_options::notify;
using boost::program_options::options_description;
using boost::program_options::positional_options_description;
using boost::program_options::store;
using boost::program_options::variables_map;

using execHelper::config::VariablesMap;

namespace execHelper::config {
OptionDescriptions::OptionDescriptions() noexcept { ; }

auto OptionDescriptions::getOptionDescriptions() const noexcept
    -> options_description {
    return m_optionDescription;
}

auto OptionDescriptions::setPositionalArgument(
    const OptionInterface& option) noexcept -> bool {
    m_positional = option.getId();
    return true;
}

auto OptionDescriptions::getOptionsMap(VariablesMap& variablesMap,
                                       const Argv& argv,
                                       bool allowUnregistered) const noexcept
    -> bool {
    variables_map optionsMap;
    auto commandLineParser =
        command_line_parser(static_cast<int>(argv.getArgc()), argv.getArgv());
    commandLineParser.options(m_optionDescription);

    // Assign positional arguments
    positional_options_description positionalOptionsDesc;
    if(m_positional) {
        positionalOptionsDesc.add(m_positional.value().c_str(), -1);
        commandLineParser.positional(positionalOptionsDesc);
    }
    if(allowUnregistered) {
        commandLineParser.allow_unregistered();
    }

    try {
        store(commandLineParser.run(), optionsMap);
    } catch(const boost::program_options::unknown_option& e) {
        user_feedback_error(
            "Could not parse command line arguments: " << e.what());
        return false;
    } catch(const std::exception& e) {
        LOG(error) << e.what();
        return false;
    }

    notify(optionsMap);
    toMap(variablesMap, optionsMap);
    return true;
}

void OptionDescriptions::toMap(VariablesMap& variablesMap,
                               const variables_map& optionsMap) const noexcept {
    for(const auto& option : optionsMap) {
        if(m_options.count(option.first) > 0U) {
            m_options.at(option.first)->toMap(variablesMap, optionsMap);
        }
    }
}
} // namespace execHelper::config
