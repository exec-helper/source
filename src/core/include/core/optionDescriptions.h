#ifndef __OPTION_DESCRIPTIONS_H__
#define __OPTION_DESCRIPTIONS_H__

#include <boost/program_options.hpp>

#include "options.h"

namespace execHelper {
    namespace core {
        class OptionDescriptions {
            public:
                OptionDescriptions() noexcept;

                boost::program_options::options_description getOptionDescriptions() const noexcept;

                template<typename T>
                void addOption(const std::string& options, const std::string& explanation, bool multitoken = false) noexcept {
                    if(multitoken) {
                        m_descriptions(options.c_str(), boost::program_options::value<T>()->multitoken(), explanation.c_str());
                    } else {
                        m_descriptions(options.c_str(), boost::program_options::value<T>(), explanation.c_str());
                    }
                }

                boost::program_options::variables_map getOptionsMap(int argc, const char* const* argv, bool allowUnregistered = false) const noexcept;

            private:
                boost::program_options::options_description m_optionDescription;
                boost::program_options::options_description_easy_init m_descriptions;
        };
    }
}

#endif  /* __OPTION_DESCRIPTIONS_H__ */
