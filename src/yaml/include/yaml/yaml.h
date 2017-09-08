#ifndef __YAML_H__
#define __YAML_H__

#include <memory>
#include <string>
#include <vector>

#include "config/configInputFile.h"
#include "config/path.h"
#include "config/settingsNode.h"

#include "yamlWrapper.h"

namespace execHelper { 
    namespace yaml {
        /**
         * \brief   Interface to reading YAML files
         */
        class Yaml : public config::ConfigInputFile {
            public:
                /**
                 * Constructor
                 *
                 * \param[in] file  Path to the file to parse
                 */
                explicit Yaml(const config::Path& file);

                /**
                 * Constructor
                 *
                 * \param[in] yamlConfig    The content to parse
                 */
                explicit Yaml(const std::string& yamlConfig);

                Yaml(const Yaml& other) = delete;
                Yaml(Yaml&& other) noexcept = delete;
                ~Yaml() override = default;

                Yaml& operator=(const Yaml& other) = delete;
                Yaml& operator=(Yaml&& other) noexcept = delete;

                /**
                 * Returns the value associated with the given key list
                 *
                 * \param keys  A collection of keys to follow
                 * \returns The associated value
                 */
                std::string getValue(const std::initializer_list<std::string>& keys);

                /**
                 * Returns the collection of values associated with the given key list
                 *
                 * \param keys  A collection of keys to follow
                 * \return  The associated values
                 */
                std::vector<std::string> getValueCollection(const std::initializer_list<std::string>& keys);

                bool getTree(const std::initializer_list<std::string>& keys, config::SettingsNode* settings) const noexcept override;

            private:
                YamlWrapper m_yaml;
        };
    } // namespace yaml
} // namespace execHelper

#endif  /*__YAML_H__*/
