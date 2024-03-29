#ifndef __YAML_WRAPPER_H__
#define __YAML_WRAPPER_H__

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

#include "config/path.h"
#include "config/settingsNode.h"

namespace execHelper::yaml {
/**
 * \brief   Wrapper for yaml-cpp
 */
class YamlWrapper {
  public:
    /**
     * Constructor
     *
     * \param[in] file  The file to load
     * \throws YAML::BadFile    If the given file does not exist
     */
    explicit YamlWrapper(const config::Path& file);

    /**
     * Constructor
     *
     * \param[in] yamlConfig    The content to parse
     */
    explicit YamlWrapper(const std::string& yamlConfig);

    /**
     * Copy constructor
     *
     * \param[in] other The object to copy from
     */
    YamlWrapper(const YamlWrapper& other) = delete;

    /**
     * Move constructor
     *
     * \param[in] other The object to move from
     */
    YamlWrapper(YamlWrapper&& other) noexcept = delete;

    ~YamlWrapper() = default;

    /**
     * Copy assignment operator
     *
     * \param[in] other The object to assign from
     * \returns A reference to this object
     */
    auto operator=(const YamlWrapper& other) -> YamlWrapper& = delete;

    /**
     * Move assignment operator
     *
     * \param[in] other the object to move assign from
     * \returns a reference to this object
     */
    auto operator=(YamlWrapper&& other) noexcept -> YamlWrapper& = delete;

    /**
     * Swap contents
     *
     * \param[in] other The object to swap with
     */
    void swap(const YamlWrapper& other) noexcept;

    /**
     * Returns the value associated with the keys as the given type
     *
     * \param[in] keys  A collection of keys to follow
     * \returns The associated value
     */
    template <typename T>
    T get(const std::initializer_list<config::SettingsKey>& keys) const {
        YAML::Node node = std::accumulate(
            keys.begin(), keys.end(), Clone(m_node),
            [](const auto& node, const auto& key) { return node[key]; });
        return node.as<T>();
    }

    /**
     * Returns the node below the given key structure
     *
     * \param[in] keys  A collection of keys to follow
     * \returns The node associated with the given key structure
     */
    YAML::Node getSubNode(const std::initializer_list<std::string>& keys) const;

    // Convenience wrapper for parsing the whole tree
    /**
     * Parses the tree below the given keys and writes it to the given settings
     *
     * \param[in] keys  The keys to write to the settings
     * \param[out] settings  The settings to write the parsed structure to
     * \returns True    If the parsing was successful
     *          False   Otherwise
     */
    bool getTree(const std::initializer_list<std::string>& keys,
                 config::SettingsNode* settings) const noexcept;

    /**
     * Parses the given node and writes it to the given settings
     *
     * \param[in] rootNode  The node to start from
     * \param[out] settings  The settings to write the parsed structure to
     * \returns True    If the parsing was successful
     *          False   Otherwise
     */
    static bool getTree(const YAML::Node& rootNode,
                        config::SettingsNode* settings) noexcept;

  private:
    /**
     * Parse the given node and add it to the given settings under the given key
     * structure
     *
     * \param[in] node  The node to parse
     * \param[out] yamlNode  The root settings
     * \param[in] keys  The key structure of the root settings under which to
     * write the parsed structure \returns True    If the subtree was
     * successfully constructed and added False   Otherwise
     */
    static bool getSubTree(const YAML::Node& node,
                           config::SettingsNode* yamlNode,
                           const config::SettingsKeys& keys) noexcept;

    YAML::Node m_node;
};
} // namespace execHelper::yaml

#endif /* __YAML_WRAPPER_H__ */
