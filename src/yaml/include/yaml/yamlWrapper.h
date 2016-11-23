#ifndef __YAML_WRAPPER_H__
#define __YAML_WRAPPER_H__

#include <string>
#include <iostream>

#include <yaml-cpp/yaml.h>

namespace execHelper { 
    namespace core {
        // Struct is mainly meant to identify the correct constructor to use
        struct YamlWrapperFile {
            std::string file;
        };

        class YamlWrapper {
            public:
                YamlWrapper(const YamlWrapperFile& file);
                YamlWrapper(const std::string& yamlConfig);

                size_t getSize(const std::string& key) const;

                template<typename T>
                T get(const std::initializer_list<std::string>& keys) {
                    YAML::Node node = Clone(m_node);
                    for(const auto& key : keys) {
                        node = node[key];
                    }
                    return node.as<T>();
                }

            private:
                const YAML::Node m_node;
        };
    }
}

#endif  /* __YAML_WRAPPER_H__ */
