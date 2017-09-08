#ifndef __CONFIG_INPUT_FILE_H__
#define __CONFIG_INPUT_FILE_H__

#include <string>

#include "config/settingsNode.h"

namespace execHelper {
    namespace config {
        /**
         * \brief Interface for configuration files
         */
        class ConfigInputFile {
            public:
		/*! @copydoc core::Argv::Argv(const Argv&)
		 * \note Deleted
		 */
                ConfigInputFile(const ConfigInputFile& other) = delete;

		/*! @copydoc core::Argv::Argv(Argv&&)
		 * \note Deleted
		 */
                ConfigInputFile(ConfigInputFile&& other) noexcept = delete;

                virtual ~ConfigInputFile() = default;

		/*! @copydoc core::Argv::operator=(const Argv&)
		 * \note Deleted
		 */
                ConfigInputFile& operator=(const ConfigInputFile& other) = delete;

		/*! @copydoc core::Argv::operator=(Argv&&)
		 * \note Deleted
		 */
                ConfigInputFile& operator=(ConfigInputFile&& other) noexcept = delete;

                /**
                 * Returns the settings under the node defined by keys
                 *
                 * \param[in] keys  The path to the root node to get the tree from
                 * \param[in] settings  The settings node to fill
                 * \return  True    If the settings node was successfully filled
                 *          False   Otherwise
                 */
                virtual bool getTree(const std::initializer_list<std::string>& keys, SettingsNode* settings) const noexcept = 0;

            protected:
                ConfigInputFile() = default;
        };
    } // namespace config
} // namespace execHelper

#endif  /* __CONFIG_INPUT_FILE_H__ */
