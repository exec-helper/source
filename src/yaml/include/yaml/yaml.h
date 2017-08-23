#ifndef __YAML_H__
#define __YAML_H__

#include <memory>
#include <string>
#include <vector>

#include "config/configInputFile.h"
#include "config/settingsNode.h"

#include "yamlWrapper.h"

namespace execHelper { 
    namespace yaml {
        // Struct is mainly meant to identify the correct constructor to use
        using YamlFile = YamlWrapperFile;

        class Yaml : public config::ConfigInputFile {
            public:
                explicit Yaml(const YamlFile& file);
                explicit Yaml(const std::string& yamlConfig);

                Yaml(const Yaml& other) = delete;
                Yaml(Yaml&& other) noexcept = delete;
                ~Yaml() override = default;

                Yaml& operator=(const Yaml& other) = delete;
                Yaml& operator=(Yaml&& other) noexcept = delete;

                std::string getValue(const std::initializer_list<std::string>& keys);
                std::vector<std::string> getValueCollection(const std::initializer_list<std::string>& keys);

                bool getTree(const std::initializer_list<std::string>& keys, config::SettingsNode* settings) const noexcept override;

            private:
                YamlWrapper m_yaml;
        };
    } // namespace yaml
} // namespace execHelper

#endif  /*__YAML_H__*/
