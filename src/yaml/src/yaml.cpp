#include "yaml.h"

#include <iostream>

#include "yamlWrapper.h"

using std::string;
using std::vector;
using std::initializer_list;

using execHelper::config::SettingsNode;

namespace execHelper { namespace yaml {
    Yaml::Yaml(const YamlFile& file)
    {
        YamlWrapperFile wrapperFile;
        wrapperFile.file = file.file;
        m_yaml.reset(new YamlWrapper(wrapperFile));
    }

    Yaml::Yaml(const string& yamlConfig) :
        m_yaml(new YamlWrapper(yamlConfig))
    {
        ;    
    }

    Yaml::~Yaml() {
        ;
    }

    string Yaml::getValue(const initializer_list<string>& keys) {
        return m_yaml->get<string>(keys);
    }
    
    vector<string> Yaml::getValueCollection(const initializer_list<string>& keys) {
        return m_yaml->get<vector<string>>(keys);
    }

    void Yaml::getTree(const initializer_list<string>& keys, SettingsNode& settings) const noexcept {
        return m_yaml->getTree(keys, settings);
    }
} }

