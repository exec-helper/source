#include "yamlWrapper.h"

#include <iostream>
#include <vector>
#include <memory>

#include <yaml-cpp/yaml.h>

using std::string;
using std::vector;
using std::initializer_list;
using std::unique_ptr;

using execHelper::config::SettingsNode;

namespace execHelper { namespace yaml {
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

    SettingsNode YamlWrapper::getTree(const initializer_list<string>& keys) const noexcept {
        const YAML::Node& node = get<YAML::Node>(keys);
        return getTree(node);
    }

    SettingsNode YamlWrapper::getTree(const YAML::Node& rootNode) noexcept {
        YAML::Node node = Clone(rootNode);
        SettingsNode yamlNode;
        yamlNode.key = "<root>";
        getSubTree(node, yamlNode);
        return yamlNode;
    }

    void YamlWrapper::getSubTree(const YAML::Node& node, SettingsNode& yamlNode) noexcept {
        for(const auto& element : node) {
            if(! yamlNode.values) {
                yamlNode.values.reset(new vector<SettingsNode>());
            }
            yamlNode.values->push_back(SettingsNode()); 
            yamlNode.values->back().key = element.first.as<string>();
            if(element.second.IsMap()) {
                YamlWrapper::getSubTree(element.second, yamlNode.values->back());
            } else {
                yamlNode.values->back().values.reset(new vector<SettingsNode>());
                if(element.second.size() == 0) {
                    yamlNode.values->back().values->push_back(SettingsNode());
                    yamlNode.values->back().values->back().key = element.second.as<string>();
                }
                for(const auto& el : element.second) {
                    yamlNode.values->back().values->push_back(SettingsNode());
                    yamlNode.values->back().values->back().key = el.as<string>();
                }
            }
        }
    }
} }
