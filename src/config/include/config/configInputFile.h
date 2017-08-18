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
                ConfigInputFile(const ConfigInputFile& other) = delete;
                ConfigInputFile(ConfigInputFile&& other) noexcept = delete;

                virtual ~ConfigInputFile() = default;

                ConfigInputFile& operator=(const ConfigInputFile& other) = delete;
                ConfigInputFile& operator=(ConfigInputFile&& other) noexcept = delete;

                /**
                 * Returns the settings under the node defined by keys
                 *
                 * \param keys  The path to the root node to get the tree from
                 * \param settings  The settings node to fill
                 * \return  True    If the settings node was successfully filled
                 *          False   Otherwise
                 */
                virtual bool getTree(const std::initializer_list<std::string>& keys, SettingsNode* settings) const noexcept = 0;

            protected:
                ConfigInputFile() = default;
        };
    }
}

#endif  /* __CONFIG_INPUT_FILE_H__ */
