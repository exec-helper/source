#include "yamlWrapper.h"

#include <iostream>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "log/assertions.h"

#include "logger.h"

using std::initializer_list;
using std::string;

using execHelper::config::Path;
using execHelper::config::SettingsKeys;
using execHelper::config::SettingsNode;
using execHelper::config::SettingsValue;

namespace execHelper::yaml {
YamlWrapper::YamlWrapper(const Path& file)
    : m_node(YAML::LoadFile(file.string())) {
    ;
}

YamlWrapper::YamlWrapper(const string& yamlConfig)
    : m_node(YAML::Load(yamlConfig)) {
    ;
}

YamlWrapper::YamlWrapper(const YamlWrapper& other)
    : m_node(Clone(other.m_node)) {
    ;
}

YamlWrapper::YamlWrapper(YamlWrapper&& other) noexcept
    : m_node(Clone(other.m_node)) {
    ;
}

auto YamlWrapper::operator=(const YamlWrapper& other) -> YamlWrapper& {
    if(this != &other) {
        m_node = Clone(other.m_node);
    }
    return *this;
}

auto YamlWrapper::operator=(YamlWrapper&& other) noexcept -> YamlWrapper& {
    swap(other);
    return *this;
}

void YamlWrapper::swap(const YamlWrapper& other) noexcept {
    try {
        m_node = Clone(other.m_node);
    } catch(const YAML::InvalidNode&) {
        LOG(fatal) << "Swap failed";
        expectsMessage(false, "Should not get here");
    }
}

auto YamlWrapper::getSubNode(
    const std::initializer_list<std::string>& keys) const -> YAML::Node {
    return std::accumulate(
        keys.begin(), keys.end(), Clone(m_node),
        [](const auto& node, const auto& key) { return node[key]; });
}

auto YamlWrapper::getTree(const initializer_list<string>& keys,
                          SettingsNode* settings) const noexcept -> bool {
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

auto YamlWrapper::getTree(const YAML::Node& rootNode,
                          SettingsNode* settings) noexcept -> bool {
    YAML::Node node = Clone(rootNode);
    return getSubTree(node, settings, {});
}

auto YamlWrapper::getSubTree(const YAML::Node& node, SettingsNode* yamlNode,
                             const SettingsKeys& keys) noexcept -> bool {
    YAML::NodeType::value type = YAML::NodeType::Null;
    try {
        type = node.Type();
    } catch(const YAML::InvalidNode&) {
        expectsMessage(false, "Should not get here");
        LOG(error) << "Using an invalid YAML node";
        return false;
    }
    switch(type) {
    case YAML::NodeType::Null:
    case YAML::NodeType::Undefined:
        break;
    case YAML::NodeType::Scalar:
        try {
            if(!yamlNode->add(keys, node.as<string>())) {
                LOG(warning) << "Failed to add key '" << keys.back() << "'";
            }
        } catch(const YAML::TypedBadConversion<string>&) {
            return false;
        } catch(const YAML::InvalidNode&) {
            return false;
        }
        break;
    case YAML::NodeType::Map:
        for(const auto& element : node) {
            SettingsValue key;
            try {
                key = element.first.as<string>();
            } catch(const YAML::TypedBadConversion<string>&) {
                return false;
            } catch(const YAML::InvalidNode&) {
                return false;
            }

            if(!yamlNode->add(keys, key)) {
                LOG(warning) << "Failed to add key '" << key << "'";
            }
            SettingsKeys newKeys = keys;
            newKeys.push_back(key);
            if(!YamlWrapper::getSubTree(element.second, yamlNode, newKeys)) {
                return false;
            }
        }
        break;
    case YAML::NodeType::Sequence:
        if(!std::all_of(node.begin(), node.end(),
                        [&yamlNode, &keys](const auto& element) {
                            return YamlWrapper::getSubTree(element, yamlNode,
                                                           keys);
                        })) {
            return false;
        }
        break;
    }
    return true;
}
} // namespace execHelper::yaml
