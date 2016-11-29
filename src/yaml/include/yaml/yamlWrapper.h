#ifndef __YAML_WRAPPER_H__
#define __YAML_WRAPPER_H__

#include <string>
#include <iostream>

#include <yaml-cpp/yaml.h>

#include "config/settingsNode.h"

namespace execHelper { 
    namespace yaml {
        // Struct is mainly meant to identify the correct constructor to use
        struct YamlWrapperFile {
            std::string file;
        };

        class YamlWrapper {
            public:
                YamlWrapper(const YamlWrapperFile& file);
                YamlWrapper(const std::string& yamlConfig);

                template<typename T>
                T get(const std::initializer_list<std::string>& keys) const {
                    YAML::Node node = Clone(m_node);
                    for(const auto& key : keys) {
                        node = node[key];
                    }
                    return node.as<T>();
                }

                // Convenience wrapper for parsing the whole tree
                bool getTree(const std::initializer_list<std::string>& keys, config::SettingsNode& settings) const noexcept;
                static bool getTree(const YAML::Node& rootNode, config::SettingsNode& settings) noexcept;

            private:
                static void getSubTree(const YAML::Node& node, config::SettingsNode& yamlNode);

                const YAML::Node m_node;
        };
    }
}

#endif  /* __YAML_WRAPPER_H__ */
