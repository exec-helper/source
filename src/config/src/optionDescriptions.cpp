#include "optionDescriptions.h"

#include "log/log.h"
#include "logger.h"
#include <stdexcept>

using std::invalid_argument;
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
    const OptionInterface& option) noexcept -> void {
    m_positional = option.getId();
}

auto OptionDescriptions::getOptionsMap(VariablesMap& variablesMap,
                                       const Args& args,
                                       bool allowUnregistered) const noexcept
    -> bool {
    variables_map optionsMap;
    auto commandLineParser =
        command_line_parser(static_cast<int>(args.size()), args.data());
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
    try {
        toMap(variablesMap, optionsMap);
    } catch(const std::invalid_argument& e) {
        user_feedback_error(e.what());
        return false;
    }
    return true;
}

void OptionDescriptions::toMap(VariablesMap& variablesMap,
                               const variables_map& optionsMap) const {
    for(const auto& option : optionsMap) {
        if(m_options.count(option.first) > 0U) {
            try {
                m_options.at(option.first)->toMap(variablesMap, optionsMap);
            } catch(const boost::bad_any_cast& e) {
                LOG(error) << "Failed to parse '" << option.first
                           << "': " << e.what();
                throw invalid_argument(
                    string("Failed to parse value for option '")
                        .append(option.first));
            } catch(const invalid_argument& e) {
                LOG(error) << e.what();
                throw invalid_argument(
                    string("Failed to parse value for option '")
                        .append(option.first));
            }
        }
    }
}
} // namespace execHelper::config
