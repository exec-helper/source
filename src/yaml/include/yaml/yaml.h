#ifndef __YAML_H__
#define __YAML_H__

#include <string>
#include <vector>
#include <sstream>
#include <memory>

// Yaml classes are being wrapped using pimpl 
namespace execHelper {
    namespace core {
        class YamlWrapper;
    }
}

namespace execHelper { 
    namespace core {
        // Struct is mainly meant to identify the correct constructor to use
        struct YamlFile {
            std::string file;
        };

        class Yaml {
            public:
                Yaml(const YamlFile& file);
                Yaml(const std::string& yamlConfig);
                ~Yaml();

                size_t getSize(const std::string& key) const;

                std::string getValue(const std::initializer_list<std::string>& keys);
                std::vector<std::string> getValueCollection(const std::initializer_list<std::string>& keys);

            private:
                YamlWrapper* m_yaml;
        };
    }
}

#endif  /*__YAML_H__*/
