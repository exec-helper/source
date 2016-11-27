#ifndef __YAML_H__
#define __YAML_H__

#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include "config/settingsNode.h"
#include "config/configInputFile.h"

// Yaml classes are being wrapped using pimpl 
namespace execHelper {
    namespace yaml {
        class YamlWrapper;
    }
}

namespace execHelper { 
    namespace yaml {
        // Struct is mainly meant to identify the correct constructor to use
        struct YamlFile {
            std::string file;
        };

        class Yaml : public config::ConfigInputFile {
            public:
                Yaml(const YamlFile& file);
                Yaml(const std::string& yamlConfig);
                virtual ~Yaml();

                std::string getValue(const std::initializer_list<std::string>& keys);
                std::vector<std::string> getValueCollection(const std::initializer_list<std::string>& keys);

                virtual void getTree(const std::initializer_list<std::string>& keys, config::SettingsNode& settings) const noexcept;

            private:
                std::unique_ptr<YamlWrapper> m_yaml;
        };
    }
}

#endif  /*__YAML_H__*/
