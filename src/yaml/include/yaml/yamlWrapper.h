#ifndef __YAML_WRAPPER_H__
#define __YAML_WRAPPER_H__

#include <iostream>
#include <string>

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

#include "config/path.h"
#include "config/settingsNode.h"

namespace execHelper { 
    namespace yaml {
        /**
         * \brief   Wrapper for yaml-cpp
         */
        class YamlWrapper {
            public:
                /**
                 * Constructor
                 *
                 * \param file  The file to load
                 * \throws YAML::BadFile    If the given file does not exist
                 */
                explicit YamlWrapper(const config::Path& file);
                explicit YamlWrapper(const std::string& yamlConfig);

                YamlWrapper(const YamlWrapper& other);
                YamlWrapper(YamlWrapper&& other) noexcept;
                ~YamlWrapper() = default;

                YamlWrapper& operator=(const YamlWrapper& other);
                YamlWrapper& operator=(YamlWrapper&& other) noexcept;

                /**
                 * Swap contents
                 *
                 * \param other The other object to swap with
                 */
                void swap(const YamlWrapper& other) noexcept;

                /**
                 * Returns the value associated with the keys as the given type
                 *
                 * \param keys  A collection of keys to follow
                 * \returns The associated value
                 */
                template<typename T>
                T get(const std::initializer_list<std::string>& keys) const {
                    YAML::Node node = Clone(m_node);
                    for(const auto& key : keys) {
                        node = node[key];
                    }
                    return node.as<T>();
                }

                /**
                 * Returns the node below the given key structure
                 *
                 * \param keys  A collection of keys to follow
                 * \returns The node associated with the given key structure
                 */
                YAML::Node getSubNode(const std::initializer_list<std::string>& keys) const;

                // Convenience wrapper for parsing the whole tree
                /**
                 * Parses the tree below the given keys and writes it to the given settings
                 *
                 * \param keys  The keys to write to the settings
                 * \param settings  The settings to write the parsed structure to
                 * \returns True    If the parsing was successful
                 *          False   Otherwise
                 */
                bool getTree(const std::initializer_list<std::string>& keys, config::SettingsNode* settings) const noexcept;

                /**
                 * Parses the given node and writes it to the given settings
                 * 
                 * \param rootNode  The node to start from
                 * \param settings  The settings to write the parsed structure to
                 * \returns True    If the parsing was successful
                 *          False   Otherwise
                 */
                static bool getTree(const YAML::Node& rootNode, config::SettingsNode* settings) noexcept;

            private:
                /**
                 * Parse the given node and add it to the given settings under the given key structure
                 *
                 * \param node  The node to parse
                 * \param yamlNode  The root settings
                 * \param keys  The key structure of the root settings under which to write the parsed structure
                 */
                static void getSubTree(const YAML::Node& node, config::SettingsNode* yamlNode, const config::SettingsNode::SettingsKeys& keys) noexcept;

                YAML::Node m_node;
        };
    } // namespace yaml
} // namespace execHelper

#endif  /* __YAML_WRAPPER_H__ */
