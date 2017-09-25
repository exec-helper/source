#ifndef __SETTINGS_NODE_H__
#define __SETTINGS_NODE_H__

#include <memory>
#include <string>
#include <vector>

#include <boost/optional/optional.hpp>

namespace execHelper {
    namespace config {
        /**
         * \brief A class containing a configuration hierarchy
         */
        class SettingsNode {
            public:
                using SettingsKey = std::string; //!< The settings key type
                using SettingsKeys = std::vector<SettingsKey>; //!< A SettingsKey collection

                using SettingsValue = SettingsKey; //!< The settings value type
                using SettingsValues = std::vector<SettingsValue>; //!< A SettingsValue collection

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
                 * Access the direct child element associated with the given key
                 *
                 * \pre #contains() == true
                 * \warning Causes undefined behaviour if the precondition is not met.
                 * \param[in] key   The child element key to return
                 * \returns A const reference to the node with the given key
                 */
                const SettingsNode& operator[](const SettingsKey& key) const noexcept;

                /**
                 * Gets the key associated with the root of this node
                 *
                 * \returns The associated key
                 */ 
                SettingsKey key() const noexcept;

                /**
                 * Get the values associated with the root of this node
                 *
                 * \returns The associated values
                 */
                SettingsValues values() const noexcept;

                /**
                 * Returns whether the given key exists as a direct child of this node
                 *
                 * \param[in] key   The key to search for
                 * \returns True if the key is a value of this node
                 *          False otherwise
                 */
                bool contains(const SettingsKey& key) const noexcept;

                /**
                 * Returns whether the given hierarchy keys path exist in the given order starting from the root of this node
                 *
                 * \param[in] keys    A hierarchy key path
                 * \returns True if the ordered keys exist as a child of this node
                 *          False otherwise
                 */
                bool contains(const SettingsKeys& keys) const noexcept;

                /**
                 * Get the direct values associated with the given hierarchy key path
                 *
                 * \param[in] keys    A hierarchy key path
                 * \returns     The associated values if the given hierarchy key path exists
                 *              boost::none otherwise
                 */
                boost::optional<SettingsValues> get(const SettingsKeys& keys) const noexcept;

                /**
                 * Get the direct values associated with the given hierarchy key path. If the given hierarchy key path does not exist, the given default value is returned.
                 *
                 * \param[in] keys    A hierarchy key path
                 * \param[in] defaultValue  The default value
                 * \return  The associated values if the given hierarchy key path exists
                 *          The given default value otherwise
                 */
                SettingsValues get(const SettingsKeys& keys, const SettingsValues& defaultValue) const noexcept;

                /**
                 * Add the given value as a direct child of this root node
                 *
                 * \warning Passing duplicate values results in undefined behaviour
                 * \param[in] newValue  The new value to add
                 * \returns True if the value was added successfully
                 *          False otherwise
                 */
                bool add(const SettingsValue& newValue) noexcept;

                /**
                 * Add the given value in the given hierarchy key path. The hierarchy key path is created if it does not exist.
                 *
                 * \warning Passing duplicate values results in undefined behaviour
                 * \param[in] keys  A hierarchy key path
                 * \param[in] newValue  The new value to add
                 * \returns True if the value was added successfully
                 *          False otherwise
                 */
                bool add(const SettingsKeys& keys, const SettingsValue& newValue) noexcept;

                /*! @copydoc add(const SettingsValues&)
                 */
                bool add(const std::initializer_list<SettingsValue>& newValues) noexcept;

                /*! @copydoc add(const SettingsKeys&, const SettingsValues&)
                 */
                bool add(const SettingsKeys& keys, const std::initializer_list<SettingsValue>& newValues) noexcept;

                /**
                 * Add the given values as direct childs to this root node
                 *
                 * \warning Passing duplicate values results in undefined behaviour
                 * \param[in] newValues The new values to add
                 * \returns True if the value was added successfully
                 *          False otherwise
                 */
                bool add(const SettingsValues& newValues) noexcept;

                /**
                 * Add the given values at the given hierarchy key path
                 * \warning Passing duplicate values results in undefined behaviour
                 * \param[in] keys  The hierarchy key path
                 * \param[in] newValues The new values to add
                 * \returns True if the value was added successfully
                 *          False otherwise
                 */
                bool add(const SettingsKeys& keys, const SettingsValues& newValues) noexcept;

                /**
                 * Remove the direct child associated with the given key
                 *
                 * \param[in] key   The key
                 * \returns True if the key was successfully removed. Note: if the key does not exist, it is considered to be removed successfully
                 *          False otherwise
                 */
                bool clear(const SettingsKey& key) noexcept;

                /**
                 * Remove the child at the end of the given hierarchy key path
                 *
                 * \param[in] keys  The hierarchy key path
                 * \returns True if the key was successfully remove. Note: if the key does not exist, it is considered to be removed successfully
                 *          False otherwise
                 */
                bool clear(const SettingsKeys& keys) noexcept;

                /**
                 * Swap the other element with this one
                 *
                 * @param[in] other The element to swap with
                 */
                void swap(SettingsNode& other) noexcept;

            private:
                using SettingsNodeCollection = std::vector<SettingsNode>; //!< A collection of nodes

                /**
                 * Make a deep copy of the content of other to this node
                 *
                 * \param[in] other The other node to copy
                 */
                void deepCopy(const SettingsNode& other) noexcept;

                /**
                 * Get the node with the given key
                 *
                 * \pre #contains() == true
                 * \warning This function is undefined if the precondition is not fulfilled
                 * \param[in] key   The key
                 * \return  A pointer to the node with the associated key
                 */
                SettingsNode* at(const SettingsKey& key) noexcept;

                /**
                 * Get the node with the given key
                 *
                 * \pre #contains() == true
                 * \warning This function is undefined if the precondition is not fulfilled
                 * \param[in] key   The key
                 * \return  A pointer to the node with the associated key
                 */
                const SettingsNode* at(const SettingsKey& key) const noexcept;

                SettingsKey m_key; //!< The root key associated with this node
                std::unique_ptr<SettingsNodeCollection> m_values; //!< The value hierarchy associated with this node
        };

        /**
         * Streaming operator for settings nodes
         * 
         * \param[in] os    The stream to stream to
         * \param[in] settings  The settings to add to the stream
         * \returns os
         */
        std::ostream& operator<<( std::ostream& os, const SettingsNode& settings) noexcept;
    } // namespace config
} // namespace execHelper

#endif /* __SETTINGS_NODE_H__ */
