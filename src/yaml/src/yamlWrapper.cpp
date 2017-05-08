#include "yamlWrapper.h"

#include <iostream>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "log/log.h"

using std::string;
using std::initializer_list;

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
        getSubTree(node, settings, {});
        return true;
    }

    void YamlWrapper::getSubTree(const YAML::Node& node, SettingsNode& yamlNode, const SettingsNode::SettingsKeys& keys) noexcept {
        switch(node.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                break;
            case YAML::NodeType::Scalar:
                yamlNode.add(keys, node.as<string>());
                break;
            case YAML::NodeType::Map:
                for(const auto& element : node) {
                    SettingsNode::SettingsValue key = element.first.as<string>();
                    yamlNode.add(keys, key);

                    SettingsNode::SettingsKeys newKeys = keys;
                    newKeys.push_back(key);
                    YamlWrapper::getSubTree(element.second, yamlNode, newKeys);
                }
                break;
            case YAML::NodeType::Sequence:
                for(const auto& element : node) {
                    YamlWrapper::getSubTree(element, yamlNode, keys);
                }
                break;
        }
    }
} }
