/// @file
/// @brief Class @ref cubos::core::memory::TypeMap.
/// @ingroup core-memory

#pragma once

#include <unordered_map>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::memory
{
    /// @brief A map that stores values of type @p V, using reflection types as keys.
    /// @tparam V Values type.
    /// @ingroup core-memory
    template <typename V>
    class TypeMap
    {
    public:
        /// @brief Sets the value associated to the given type.
        /// @note If an entry already exists, it is replaced.
        /// @param type Type.
        /// @param value Value.
        void insert(const reflection::Type& type, V value)
        {
            mMap.insert_or_assign(&type, std::move(value));
        }

        /// @brief Sets the value associated to the given type.
        /// @note If an entry already exists, it is replaced.
        /// @tparam K Type.
        /// @param value Value.
        template <typename K>
        void insert(V value)
        {
            this->insert(reflection::reflect<K>(), std::move(value));
        }

        /// @brief Removes the entry associated to the given type.
        /// @param type Type.
        /// @return Whether the entry was removed.
        bool erase(const reflection::Type& type)
        {
            return mMap.erase(&type) > 0;
        }

        /// @brief Removes the entry associated to the given type.
        /// @tparam K Type.
        /// @return Whether the entry was removed.
        template <typename K>
        bool erase()
        {
            return this->erase(reflection::reflect<K>());
        }

        /// @brief Checks if there's a an entry with the given type.
        /// @param type Type.
        /// @return Whether there's an entry with the given type.
        bool contains(const reflection::Type& type) const
        {
            return mMap.contains(&type);
        }

        /// @brief Checks if there's a an entry with the given type.
        /// @tparam K Type.
        /// @return Whether there's an entry with the given type.
        template <typename K>
        bool contains() const
        {
            return this->contains(reflection::reflect<K>());
        }

        /// @brief Gets the value associated to the given type.
        /// @note Aborts if @ref contains returns false.
        /// @param type Type.
        /// @return Reference to the value.
        V& at(const reflection::Type& type)
        {
            return mMap.at(&type);
        }

        /// @brief Gets the value associated to the given type.
        /// @note Aborts if @ref contains returns false.
        /// @tparam K Type.
        /// @return Reference to the value.
        template <typename K>
        V& at()
        {
            return this->at(reflection::reflect<K>());
        }

        /// @copydoc at(const reflection::Type&)
        const V& at(const reflection::Type& type) const
        {
            return mMap.at(&type);
        }

        /// @copydoc at()
        template <typename K>
        const V& at() const
        {
            return this->at(reflection::reflect<K>());
        }

        /// @brief Removes all entries from the map.
        void clear()
        {
            mMap.clear();
        }

        /// @brief Gets the number of entries in the map.
        /// @return Values count.
        std::size_t size() const
        {
            return mMap.size();
        }

        /// @brief Checks if the map is empty.
        /// @return Whether the map is empty.
        bool empty() const
        {
            return mMap.empty();
        }

        /// @brief Gets an iterator to the beginning of the map.
        /// @return Iterator.
        auto begin()
        {
            return mMap.begin();
        }

        /// @brief Gets an iterator to the end of the map.
        /// @return Iterator.
        auto end()
        {
            return mMap.end();
        }

        /// @brief Gets an iterator to the beginning of the map.
        /// @return Iterator.
        auto begin() const
        {
            return mMap.begin();
        }

        /// @brief Gets an iterator to the end of the map.
        /// @return Iterator.
        auto end() const
        {
            return mMap.end();
        }

    private:
        std::unordered_map<const reflection::Type*, V> mMap; ///< Map of values indexed by type.
    };
} // namespace cubos::core::memory
