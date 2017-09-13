#ifndef __OPTION_DESCRIPTIONS_H__
#define __OPTION_DESCRIPTIONS_H__

#include <boost/program_options.hpp>

#include "options.h"

namespace execHelper {
    namespace core {
        /**
         * \brief Represents the descriptions of the options. This description is used for interpreting e.g. command line arguments.
         */
        class OptionDescriptions {
            public:
                OptionDescriptions() noexcept;

                /**
                 * Returns the currently registered option descriptions
                 *
                 * \returns A collection of the registered option descriptions
                 */
                boost::program_options::options_description getOptionDescriptions() const noexcept;

                /**
                 * Add an option description
                 *
                 * \param[in] options   The command line options as a comma separated list
                 * \param[in] explanation   The explanation to print for the specific option
                 * \param[in] multitoken    Whether the option is a multitoken option
                 */
                template<typename T>
                void addOption(const std::string& options, const std::string& explanation, bool multitoken = false) noexcept {
                    if(multitoken) {
                        m_descriptions(options.c_str(), boost::program_options::value<T>()->multitoken(), explanation.c_str());
                    } else {
                        m_descriptions(options.c_str(), boost::program_options::value<T>(), explanation.c_str());
                    }
                }

                /**
                 * Returns a map containing the parsed option descriptions for the given command line arguments
                 *
                 * \param[in] argc  The length of argv
                 * \param[in] argv  Collection of pointers to the beginning of the arguments
                 * \param[in] allowUnregistered Whether to allow options in argv that are not described in this option description
                 * \returns A key-value collection containing the values of the known arguments in argv
                 */
                boost::program_options::variables_map getOptionsMap(int argc, const char* const* argv, bool allowUnregistered = false) const noexcept;

            private:
                boost::program_options::options_description m_optionDescription;
                boost::program_options::options_description_easy_init m_descriptions;
        };
    } // namespace core
} // namespace execHelper

#endif  /* __OPTION_DESCRIPTIONS_H__ */
