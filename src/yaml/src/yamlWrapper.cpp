#include "yamlWrapper.h"

#include <iostream>
#include <vector>

#include <yaml-cpp/yaml.h>

using std::string;
using std::vector;

namespace execHelper { namespace core {
    YamlWrapper::YamlWrapper(const YamlWrapperFile& file) :
        m_node(YAML::LoadFile(file.file))
    {
        ;
    }

    YamlWrapper::YamlWrapper(const string& yamlConfig) :
        m_node(YAML::Load(yamlConfig))
    {
        ;
    }

    size_t YamlWrapper::getSize(const std::string& key) const {
        return m_node[key].size();
    }
} }
