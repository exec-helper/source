#include "yaml.h"

#include <iostream>

#include "logger.h"

using std::string;
using std::vector;
using std::initializer_list;

using execHelper::config::Path;
using execHelper::config::SettingsNode;

namespace execHelper { namespace yaml {
    Yaml::Yaml(const Path& file) :
        m_yaml(file)
    {
        ;
    }

    Yaml::Yaml(const string& yamlConfig) :
        m_yaml(yamlConfig)
    {
        ;
    }

    string Yaml::getValue(const initializer_list<string>& keys) {
        try {
            return m_yaml.get<string>(keys);
        } catch(YAML::Exception& e) {
            LOG(error) << "Yaml parser threw an exception: " << e.what();
            return "";
        }
    }
    
    vector<string> Yaml::getValueCollection(const initializer_list<string>& keys) {
        try {
            return m_yaml.get<vector<string>>(keys);
        } catch(YAML::Exception& e) {
            LOG(error) << "Yaml parser threw an exception: " << e.what();
            return vector<string>();
        }
    }

    bool Yaml::getTree(const initializer_list<string>& keys, SettingsNode* settings) const noexcept {
        return m_yaml.getTree(keys, settings);
    }
} // namespace yaml
} // namespace execHelper
