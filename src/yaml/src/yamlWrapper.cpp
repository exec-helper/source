#include "yamlWrapper.h"

#include <iostream>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "log/assertions.h"

#include "logger.h"

using std::initializer_list;
using std::string;

using execHelper::config::Path;
using execHelper::config::SettingsNode;

namespace execHelper { namespace yaml {
    YamlWrapper::YamlWrapper(const Path& file) :
        m_node(YAML::LoadFile(file.native()))
    {
        ;
    }

    YamlWrapper::YamlWrapper(const string& yamlConfig) :
        m_node(YAML::Load(yamlConfig))
    {
        ;
    }

    YamlWrapper::YamlWrapper(const YamlWrapper& other) :
        m_node(Clone(other.m_node))
    {
        ;
    }

    YamlWrapper:: YamlWrapper(YamlWrapper&& other) noexcept :
        m_node(Clone(other.m_node))
    {
        ;
    }

    YamlWrapper& YamlWrapper::operator=(const YamlWrapper& other) {
        m_node = Clone(other.m_node);
        return *this;
    }

    YamlWrapper& YamlWrapper::operator=(YamlWrapper&& other) noexcept {
        swap(other);
        return *this;
    }

    void YamlWrapper::swap(const YamlWrapper& other) noexcept {
        try {
            m_node = Clone(other.m_node);
        } catch(const YAML::InvalidNode&) {
            LOG(fatal) << "Swap failed";
            expects(false);
        }
    }

    YAML::Node YamlWrapper::getSubNode(const std::initializer_list<std::string>& keys) const {
        YAML::Node node = Clone(m_node);
        for(const auto& key : keys) {
            node = node[key];
        }
        return node;
    }

    bool YamlWrapper::getTree(const initializer_list<string>& keys, SettingsNode* settings) const noexcept {
        try {
            const YAML::Node& node = getSubNode(keys);
            if(node.size() == 0 || node.IsNull()) {
                return false;
            }
            return getTree(node, settings);
        } catch(YAML::Exception& e) {
            LOG(error) << "YAML parser threw error: " << e.what();
            return false;
        }
    }

    bool YamlWrapper::getTree(const YAML::Node& rootNode, SettingsNode* settings) noexcept {
        YAML::Node node = Clone(rootNode);
        return getSubTree(node, settings, {});
    }

    bool YamlWrapper::getSubTree(const YAML::Node& node, SettingsNode* yamlNode, const SettingsNode::SettingsKeys& keys) noexcept {
        switch(node.Type()) {
            case YAML::NodeType::Null:
            case YAML::NodeType::Undefined:
                break;
            case YAML::NodeType::Scalar:
                try {
                    yamlNode->add(keys, node.as<string>());
                } catch(const YAML::TypedBadConversion<string>&) {
                    return false;
                } catch(const YAML::InvalidNode&) {
                    return false;
                }
                break;
            case YAML::NodeType::Map:
                for(const auto& element : node) {
                    SettingsNode::SettingsValue key;
                    try {
                        key = element.first.as<string>();
                    } catch(const YAML::TypedBadConversion<string>&) {
                        return false;
                    } catch(const YAML::InvalidNode&) {
                        return false;
                    }

                    yamlNode->add(keys, key);
                    SettingsNode::SettingsKeys newKeys = keys;
                    newKeys.push_back(key);
                    if(! YamlWrapper::getSubTree(element.second, yamlNode, newKeys)) {
                        return false;
                    }
                }
                break;
            case YAML::NodeType::Sequence:
                for(const auto& element : node) {
                    if(!YamlWrapper::getSubTree(element, yamlNode, keys)) {
                        return false;
                    }
                }
                break;
        }
        return true;
    }
} // namespace yaml
} // namespace execHelper
