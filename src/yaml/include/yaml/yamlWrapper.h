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
        class YamlWrapper {
            public:
                // May throw YAML::BadFile exception
                explicit YamlWrapper(const config::Path& file);
                explicit YamlWrapper(const std::string& yamlConfig);

                YamlWrapper(const YamlWrapper& other);
                YamlWrapper(YamlWrapper&& other) noexcept;
                ~YamlWrapper() = default;

                YamlWrapper& operator=(const YamlWrapper& other);
                YamlWrapper& operator=(YamlWrapper&& other) noexcept;

                void swap(const YamlWrapper& other) noexcept;

                template<typename T>
                T get(const std::initializer_list<std::string>& keys) const {
                    YAML::Node node = Clone(m_node);
                    for(const auto& key : keys) {
                        node = node[key];
                    }
                    return node.as<T>();
                }

                YAML::Node getSubNode(const std::initializer_list<std::string>& keys) const;

                // Convenience wrapper for parsing the whole tree
                bool getTree(const std::initializer_list<std::string>& keys, config::SettingsNode* settings) const noexcept;
                static bool getTree(const YAML::Node& rootNode, config::SettingsNode* settings) noexcept;

            private:
                static void getSubTree(const YAML::Node& node, config::SettingsNode* yamlNode, const config::SettingsNode::SettingsKeys& keys) noexcept;

                YAML::Node m_node;
        };
    } // namespace yaml
} // namespace execHelper

#endif  /* __YAML_WRAPPER_H__ */
