#ifndef __SETTINGS_NODE_H__
#define __SETTINGS_NODE_H__

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "cast.h"

namespace execHelper {
namespace config {
using SettingsKey = std::string;               //!< The settings key type
using SettingsKeys = std::vector<SettingsKey>; //!< A SettingsKey collection

using SettingsValue = SettingsKey; //!< The settings value type
using SettingsValues =
    std::vector<SettingsValue>; //!< A SettingsValue collection

/**
 * \brief A class containing a configuration hierarchy
 */
class SettingsNode {
  public:
    /**
     * Create a new node with the given key
     *
     * \param[in] key   The key to associate with the root of the node
     */
    explicit SettingsNode(SettingsKey key) noexcept;

    /**
     * Create a new node by making a deep copy of the given other node
     *
     * \param[in] other The other node to copy
     */
    SettingsNode(const SettingsNode& other) noexcept;

    /**
     * Create a new node by moving the content of the given other node
     *
     * \param[in] other The other node to move from
     */
    SettingsNode(SettingsNode&& other) noexcept;

    ~SettingsNode() noexcept;

    /**
     * Assign a deep copy of the content of the given node to this node
     *
     * \param[in] other The other node to copy from
     * \returns A reference to this
     */
    auto operator=(const SettingsNode& other) noexcept -> SettingsNode&;

    /*! @copydoc operator=(const SettingsNode&)
     */
    auto operator=(SettingsNode&& other) noexcept -> SettingsNode&;

    /**
     * Returns whether the given node equals this node
     *
     * \param[in] other The other node to compare with
     * \returns True if the nodes are equal
     *          False otherwise
     */
    auto operator==(const SettingsNode& other) const noexcept -> bool;

    /**
     *
     * Returns whether the given node is not equal to this node
     * \param[in] other The other node to compare with
     * \returns ! #operator=="()"
     */
    auto operator!=(const SettingsNode& other) const noexcept -> bool;

    /**
     * Get the node associated with the given key. If the key does not exist, it
     * is created
     *
     * \param[in] key   The key
     * \return  The node associated with the given key
     */
    auto operator[](const SettingsKey& key) noexcept -> SettingsNode&;

    /**
     * Get the node associated with the given key
     *
     * \pre #contains() == true
     * \warning This function is undefined if the precondition is not fulfilled
     * \param[in] key   The key
     * \return  The node associated with the given key
     */
    auto operator[](const SettingsKey& key) const noexcept
        -> const SettingsNode&;

    /**
     * Gets the key associated with the root of this node
     *
     * \returns The associated key
     */
    [[nodiscard]] auto key() const noexcept -> const SettingsKey&;

    /**
     * Returns whether the given key exists as a direct child of this node
     *
     * \param[in] key   The key to search for
     * \returns True if the key is a value of this node
     *          False otherwise
     */
    [[nodiscard]] auto contains(const SettingsKey& key) const noexcept -> bool;

    /*! @copydoc contains(const SettingsKey&) const
     */
    [[nodiscard]] auto contains(const SettingsKeys& key) const noexcept -> bool;

    /**
     * Get the direct values associated with the given key path
     *
     * \param[in] key   A hierarchy key path
     * \returns     The associated values if the given hierarchy key path exists
     *              boost::none otherwise
     */
    template <typename T>
    [[nodiscard]] auto get(const SettingsKeys& key) const noexcept
        -> std::optional<T> {
        if(!contains(key)) {
            return std::nullopt;
        }
        auto valuesOpt = at(key)->values();
        if(!valuesOpt) {
            return std::nullopt;
        }
        return detail::Cast<T, SettingsValues>::cast(valuesOpt.value());
    }

    /*! @copydoc get(const SettingsKeys&) const
     */
    template <typename T>
    [[nodiscard]] inline auto get(const SettingsKey& key) const noexcept
        -> std::optional<T> {
        return get<T>(SettingsKeys({key}));
    }

    /*! @copydoc get(const SettingsKeys&) const
     */
    template <typename T>
    [[nodiscard]] inline auto get(const std::string_view& key) const noexcept
        -> std::optional<T> {
        return get<T>(std::string(key));
    }

    /**
     * Get the direct values associated with the given key path or the default
     * value it does not exist
     *
     * \param[in] key    A hierarchy key path
     * \param[in] defaultValue  The value to return if the key does not exist
     * \returns     The associated values if the given hierarchy key path exists
     *              boost::none otherwise
     */
    template <typename T>
    [[nodiscard]] auto get(const SettingsKeys& key,
                           const T& defaultValue) const noexcept -> T {
        return get<T>(key).value_or(defaultValue);
    }

    /*! @copydoc get(const SettingsKeys&, const T& defaultValue) const
     */
    template <typename T>
    [[nodiscard]] inline auto get(const SettingsKey& key,
                                  const T& defaultValue) const noexcept -> T {
        return get<T>(SettingsKeys({key}), defaultValue);
    }

    /**
     * Replace the current values of the given key with the given values
     *
     * \param[in] key   A hierarchy key path
     * \param[in] values    The new values
     * \returns     Whether the values were successfully replaced. If false,
     * there is no guarantee that the previous values are still present.
     */
    template <typename T>
    [[nodiscard]] inline auto replace(const SettingsKeys& key,
                                      const T& values) noexcept -> bool {
        if(contains(key)) {
            clear(key);
        }
        return add(key, values);
    }

    /*! @copydoc replace(const SettingsKeys&, const T&)
     */
    template <typename T>
    [[nodiscard]] inline auto replace(const SettingsKey& key,
                                      const T& values) noexcept -> bool {
        return replace(SettingsKeys({key}), values);
    }

    /**
     * Add the given value as a direct child of this root node
     *
     * \warning Passing duplicate values results in undefined behaviour
     * \param[in] newValue  The new value to add
     * \returns True if the value was added successfully
     *          False otherwise
     */
    [[nodiscard]] auto add(const SettingsValue& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsValue&)
     */
    [[nodiscard]] auto add(const SettingsValues& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsValue&)
     */
    [[nodiscard]] auto
    add(const std::initializer_list<SettingsValue>& newValue) noexcept -> bool;

    /**
     * Add the given value in the given hierarchy key path. The hierarchy key
     * path is created if it does not exist.
     *
     * \warning Passing duplicate values results in undefined behaviour
     * \param[in] key  A hierarchy key path
     * \param[in] newValue  The new value to add
     * \returns True if the value was added successfully
     *          False otherwise
     */
    [[nodiscard]] auto add(const SettingsKeys& key,
                           const SettingsValue& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    [[nodiscard]] auto add(const SettingsKey& key,
                           const SettingsValue& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    [[nodiscard]] auto
    add(const SettingsKeys& key,
        const std::initializer_list<SettingsValue>& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    [[nodiscard]] auto
    add(const std::initializer_list<SettingsKey>& key,
        const std::initializer_list<SettingsValue>& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    [[nodiscard]] auto add(const SettingsKeys& key,
                           const SettingsValues& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsKey&, const SettingsValue&)
     */
    [[nodiscard]] auto add(const SettingsKey& key,
                           const SettingsValues& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsKey&, const SettingsValue&)
     */
    [[nodiscard]] auto add(const std::initializer_list<SettingsKey>& key,
                           const SettingsValues& newValue) noexcept -> bool;

    /*! @copydoc add(const SettingsKey&, const SettingsValue&)
     */
    [[nodiscard]] auto add(const std::initializer_list<SettingsKey>& key,
                           const SettingsValue& newValue) noexcept -> bool;

    /**
     * Remove the direct child associated with the given key
     *
     * \param[in] key   The key
     * \returns True if the key was successfully removed. Note: if the key does
     * not exist, it is considered to be removed successfully False otherwise
     */
    auto clear(const SettingsKey& key) noexcept -> bool;

    /*! @copydoc clear(const SettingsKey&)
     */
    inline auto clear(const std::string_view& key) noexcept -> bool {
        return clear(SettingsKey(key));
    }

    /**
     * Remove the child at the end of the given hierarchy key path
     *
     * \param[in] keys  The hierarchy key path
     * \returns True if the key was successfully remove. Note: if the key does
     * not exist, it is considered to be removed successfully False otherwise
     */
    auto clear(const SettingsKeys& keys) noexcept -> bool;

    /**
     * Swap the other element with this one
     *
     * @param[in] other The element to swap with
     */
    void swap(SettingsNode& other) noexcept;

    /**
     * Overwrite the elements in this settings node with the key/value pairs described in the given one.
     * New keys will be added, existing keys will be overwritten with the new content.
     *
     * \param[in] newSettings   The new settings to add
     */
    void overwrite(const SettingsNode& newSettings) noexcept;

    /**
     * Get the values associated with the root of this node
     *
     * \returns The associated values if there are values associated with the root of this node
     *          boost::none otherwise
     */
    [[nodiscard]] auto values() const noexcept -> std::optional<SettingsValues>;

  private:
    using SettingsNodeCollection =
        std::vector<SettingsNode>; //!< A collection of nodes

    /**
     * Make a deep copy of the content of other to this node
     *
     * \param[in] other The other node to copy
     */
    void deepCopy(const SettingsNode& other) noexcept;

    /*! @copydoc operator[](const SettingsKey&) const
     */
    auto at(const SettingsKey& key) noexcept -> SettingsNode*;

    /*! @copydoc operator[](const SettingsKey&) const
     */
    auto at(const SettingsKey& key) const noexcept -> const SettingsNode*;

    /*! @copydoc operator[](const SettingsKey&) const
     */
    auto at(const SettingsKeys& key) noexcept -> SettingsNode*;

    /*! @copydoc operator[](const SettingsKey&) const
     */
    auto at(const SettingsKeys& key) const noexcept -> const SettingsNode*;

    SettingsKey m_key; //!< The root key associated with this node
    std::unique_ptr<SettingsNodeCollection>
        m_values; //!< The value hierarchy associated with this node
};

/**
 * Streaming operator for settings nodes
 *
 * \param[in] out    The stream to stream to
 * \param[in] settings  The settings to add to the stream
 * \returns out
 */
auto operator<<(std::ostream& out, const SettingsNode& settings) noexcept
    -> std::ostream&;
} // namespace config
} // namespace execHelper

#endif /* __SETTINGS_NODE_H__ */
