#include "yaml.h"

#include <iostream>

#include "log/log.h"
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
        try {
            m_yaml.reset(new YamlWrapper(wrapperFile));
        } catch(YAML::Exception& e) {
            LOG("Yaml parser threw error: " << e.what());
            m_yaml.reset();
        }
    }

    Yaml::Yaml(const string& yamlConfig) {  // Due to the try/catch we can not initialize this in the initializer list
        try { 
            m_yaml.reset(new YamlWrapper(yamlConfig));
        } catch(YAML::Exception& e) {
            LOG("Yaml parser threw error: " << e.what());
            m_yaml.reset();
        }
    }

    Yaml::~Yaml() {
        ;
    }

    string Yaml::getValue(const initializer_list<string>& keys) {
        if(! m_yaml) {
            return "";
        }

        try {
            return m_yaml->get<string>(keys);
        } catch(YAML::Exception& e) {
            LOG("Yaml parser threw an exception: " << e.what());
            return "";
        }
    }
    
    vector<string> Yaml::getValueCollection(const initializer_list<string>& keys) {
        if(! m_yaml) {
            return vector<string>();
        }

        try {
            return m_yaml->get<vector<string>>(keys);
        } catch(YAML::Exception& e) {
            LOG("Yaml parser threw an exception: " << e.what());
            return vector<string>();
        }
    }

    bool Yaml::getTree(const initializer_list<string>& keys, SettingsNode& settings) const noexcept {
        if(! m_yaml) {
            return false;
        }
        return m_yaml->getTree(keys, settings);
    }
} }
