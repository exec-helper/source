#ifndef __SETTINGS_NODE_H__
#define __SETTINGS_NODE_H__

#include <memory>
#include <optional>
#include <string>
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
    SettingsNode& operator=(const SettingsNode& other) noexcept;

    /*! @copydoc operator=(const SettingsNode&)
     */
    SettingsNode& operator=(SettingsNode&& other) noexcept;

    /**
     * Returns whether the given node equals this node
     *
     * \param[in] other The other node to compare with
     * \returns True if the nodes are equal
     *          False otherwise
     */
    bool operator==(const SettingsNode& other) const noexcept;

    /**
     *
     * Returns whether the given node is not equal to this node
     * \param[in] other The other node to compare with
     * \returns ! #operator=="()"
     */
    bool operator!=(const SettingsNode& other) const noexcept;

    /**
     * Get the node associated with the given key. If the key does not exist, it
     * is created
     *
     * \param[in] key   The key
     * \return  The node associated with the given key
     */
    SettingsNode& operator[](const SettingsKey& key) noexcept;

    /**
     * Get the node associated with the given key
     *
     * \pre #contains() == true
     * \warning This function is undefined if the precondition is not fulfilled
     * \param[in] key   The key
     * \return  The node associated with the given key
     */
    const SettingsNode& operator[](const SettingsKey& key) const noexcept;

    /**
     * Gets the key associated with the root of this node
     *
     * \returns The associated key
     */
    SettingsKey key() const noexcept;

    /**
     * Returns whether the given key exists as a direct child of this node
     *
     * \param[in] key   The key to search for
     * \returns True if the key is a value of this node
     *          False otherwise
     */
    bool contains(const SettingsKey& key) const noexcept;

    /*! @copydoc contains(const SettingsKey&) const
     */
    bool contains(const SettingsKeys& key) const noexcept;

    /**
     * Get the direct values associated with the given key path
     *
     * \param[in] key   A hierarchy key path
     * \returns     The associated values if the given hierarchy key path exists
     *              boost::none otherwise
     */
    template <typename T>
    std::optional<T> get(const SettingsKeys& key) const noexcept {
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
    inline std::optional<T> get(const SettingsKey& key) const noexcept {
        return get<T>(SettingsKeys({key}));
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
    T get(const SettingsKeys& key, const T& defaultValue) const noexcept {
        return get<T>(key).value_or(defaultValue);
    }

    /*! @copydoc get(const SettingsKeys&, const T& defaultValue) const
     */
    template <typename T>
    inline T get(const SettingsKey& key, const T& defaultValue) const noexcept {
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
    inline bool replace(const SettingsKeys& key, const T& values) noexcept {
        if(contains(key)) {
            clear(key);
        }
        return add(key, values);
    }

    /*! @copydoc replace(const SettingsKeys&, const T&)
     */
    template <typename T>
    inline bool replace(const SettingsKey& key, const T& values) noexcept {
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
    bool add(const SettingsValue& newValue) noexcept;

    /*! @copydoc add(const SettingsValue&)
     */
    bool add(const SettingsValues& newValue) noexcept;

    /*! @copydoc add(const SettingsValue&)
     */
    bool add(const std::initializer_list<SettingsValue>& newValue) noexcept;

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
    bool add(const SettingsKeys& key, const SettingsValue& newValue) noexcept;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    bool add(const SettingsKey& key, const SettingsValue& newValue) noexcept;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    bool add(const SettingsKeys& key,
             const std::initializer_list<SettingsValue>& newValue) noexcept;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    bool add(const std::initializer_list<SettingsKey>& key,
             const std::initializer_list<SettingsValue>& newValue) noexcept;

    /*! @copydoc add(const SettingsKeys&, const SettingsValue&)
     */
    bool add(const SettingsKeys& key, const SettingsValues& newValue) noexcept;

    /*! @copydoc add(const SettingsKey&, const SettingsValue&)
     */
    bool add(const SettingsKey& key, const SettingsValues& newValue) noexcept;

    /*! @copydoc add(const SettingsKey&, const SettingsValue&)
     */
    bool add(const std::initializer_list<SettingsKey>& key,
             const SettingsValues& newValue) noexcept;

    /*! @copydoc add(const SettingsKey&, const SettingsValue&)
     */
    bool add(const std::initializer_list<SettingsKey>& key,
             const SettingsValue& newValue) noexcept;

    /**
     * Remove the direct child associated with the given key
     *
     * \param[in] key   The key
     * \returns True if the key was successfully removed. Note: if the key does
     * not exist, it is considered to be removed successfully False otherwise
     */
    bool clear(const SettingsKey& key) noexcept;

    /**
     * Remove the child at the end of the given hierarchy key path
     *
     * \param[in] keys  The hierarchy key path
     * \returns True if the key was successfully remove. Note: if the key does
     * not exist, it is considered to be removed successfully False otherwise
     */
    bool clear(const SettingsKeys& keys) noexcept;

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
    SettingsNode* at(const SettingsKey& key) noexcept;

    /*! @copydoc operator[](const SettingsKey&) const
     */
    const SettingsNode* at(const SettingsKey& key) const noexcept;

    /*! @copydoc operator[](const SettingsKey&) const
     */
    SettingsNode* at(const SettingsKeys& key) noexcept;

    /*! @copydoc operator[](const SettingsKey&) const
     */
    const SettingsNode* at(const SettingsKeys& key) const noexcept;

    /**
     * Get the values associated with the root of this node
     *
     * \returns The associated values if there are values associated with the root of this node
     *          boost::none otherwise
     */
    std::optional<SettingsValues> values() const noexcept;

    SettingsKey m_key; //!< The root key associated with this node
    std::unique_ptr<SettingsNodeCollection>
        m_values; //!< The value hierarchy associated with this node
};

/**
 * Streaming operator for settings nodes
 *
 * \param[in] os    The stream to stream to
 * \param[in] settings  The settings to add to the stream
 * \returns os
 */
std::ostream& operator<<(std::ostream& os,
                         const SettingsNode& settings) noexcept;
} // namespace config
} // namespace execHelper

#endif /* __SETTINGS_NODE_H__ */
