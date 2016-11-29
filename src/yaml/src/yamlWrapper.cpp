#include "yamlWrapper.h"

#include <iostream>
#include <vector>
#include <memory>

#include <yaml-cpp/yaml.h>

#include "log/log.h"

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

    bool YamlWrapper::getTree(const initializer_list<string>& keys, SettingsNode& settings) const noexcept {
        try {
            const YAML::Node& node = get<YAML::Node>(keys);
            if(node.size() == 0 || node.IsNull()) {
                return false;
            }
            return getTree(node, settings);
        } catch(YAML::Exception& e) {
            LOG("YAML parser threw error: " << e.what());
            return false;
        }
    }

    bool YamlWrapper::getTree(const YAML::Node& rootNode, SettingsNode& settings) noexcept {
        YAML::Node node = Clone(rootNode);
        settings.key = "<root>";
        getSubTree(node, settings);
        return true;
    }

    void YamlWrapper::getSubTree(const YAML::Node& node, SettingsNode& yamlNode) {
        for(const auto& element : node) {
            yamlNode.values.emplace_back(SettingsNode()); 
            yamlNode.values.back().key = element.first.as<string>();
            if(element.second.IsMap()) {
                YamlWrapper::getSubTree(element.second, yamlNode.values.back());
            } else {
                if(element.second.size() == 0) {
                    yamlNode.values.back().values.emplace_back(SettingsNode());
                    yamlNode.values.back().values.back().key = element.second.as<string>();
                }
                for(const auto& el : element.second) {
                    yamlNode.values.back().values.emplace_back(SettingsNode());
                    yamlNode.values.back().values.back().key = el.as<string>();
                }
            }
        }
    }
} }
