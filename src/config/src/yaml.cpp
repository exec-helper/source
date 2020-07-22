#include "yaml.h"

#include <iostream>

#include "logger.h"

using std::initializer_list;
using std::string;
using std::vector;

using execHelper::config::Path;
using execHelper::config::SettingsNode;

namespace execHelper::yaml {
Yaml::Yaml(const Path& file) : m_yaml(file) { ; }

Yaml::Yaml(const string& yamlConfig) : m_yaml(yamlConfig) { ; }

auto Yaml::getValue(const initializer_list<string>& keys) -> string {
    try {
        return m_yaml.get<string>(keys);
    } catch(YAML::Exception& e) {
        LOG(error) << "Yaml parser threw an exception: " << e.what();
        return "";
    }
}

auto Yaml::getValueCollection(const initializer_list<string>& keys)
    -> vector<string> {
    try {
        return m_yaml.get<vector<string>>(keys);
    } catch(YAML::Exception& e) {
        LOG(error) << "Yaml parser threw an exception: " << e.what();
        return vector<string>();
    }
}

auto Yaml::getTree(const initializer_list<string>& keys,
                   SettingsNode* settings) const noexcept -> bool {
    return m_yaml.getTree(keys, settings);
}
} // namespace execHelper::yaml
