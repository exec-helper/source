#include "yaml.h"

#include "yamlWrapper.h"

using std::string;
using std::vector;

namespace execHelper { namespace core {
    Yaml::Yaml(const YamlFile& file)
    {
        YamlWrapperFile wrapperFile;
        wrapperFile.file = file.file;
        m_yaml = new YamlWrapper(wrapperFile);
    }

    Yaml::Yaml(const string& yamlConfig) :
        m_yaml(new YamlWrapper(yamlConfig))
    {
        
    }

    Yaml::~Yaml() {
        delete m_yaml;
    }

    string Yaml::getValue(const std::initializer_list<string>& keys) {
        return m_yaml->get<string>(keys);
    }
    
    vector<string> Yaml::getValueCollection(const std::initializer_list<string>& keys) {
        return m_yaml->get<vector<string>>(keys);
    }
} }

