/// @file
/// @brief Class @ref cubos::core::memory::TypeMap.
/// @ingroup core-memory

#pragma once

#include <optional>
#include <typeindex>
#include <unordered_map>

namespace cubos::core::memory
{
    /// @brief A map that stores values of type @p V, using types as keys.
    /// @tparam V Values type.
    /// @ingroup core-memory
    template <typename V>
    class TypeMap
    {
    public:
        ~TypeMap() = default;

        /// @brief Constructs an empty map.
        TypeMap() = default;

        /// @brief Move constructs.
        TypeMap(TypeMap&&) noexcept = default;

        /// @brief Sets the value associated to the given type.
        /// @param key Index of the type to use as a key.
        /// @param value Value to store.
        inline void set(std::type_index key, V value)
        {
            mMap.emplace(key, std::move(value));
        }

        /// @brief Gets the value associated to the given type.
        /// @param key Index of the type to use as a key.
        /// @return Pointer to the value or null if it isn't stored.
        inline V* at(std::type_index key)
        {
            auto it = mMap.find(key);
            if (it == mMap.end())
            {
                return nullptr;
            }
            return &it->second;
        }

        /// @brief Gets the value associated to the given type.
        /// @param key Index of the type to use as a key.
        /// @return Pointer to the value or null if it isn't stored.
        inline const V* at(std::type_index key) const
        {
            auto it = mMap.find(key);
            if (it == mMap.end())
            {
                return nullptr;
            }
            return &it->second;
        }

        /// @brief Sets the value associated to the given type.
        /// @tparam K Type to use as a key.
        /// @param value Value to store.
        template <typename K>
        inline void set(V value)
        {
            this->set(std::type_index(typeid(K)), std::move(value));
        }

        /// @brief Gets the value associated to the given type.
        /// @tparam K Type to use as a key.
        /// @return Pointer to the value or null if it isn't stored.
        template <typename K>
        inline V* at()
        {
            return this->at(std::type_index(typeid(K)));
        }

        /// @brief Gets the value associated to the given type.
        /// @tparam K Type to use as a key.
        /// @return Pointer to the value or null if it isn't stored.
        template <typename K>
        inline const V* at() const
        {
            return this->at(std::type_index(typeid(K)));
        }

        /// @brief Removes all values from the map.
        inline void clear()
        {
            mMap.clear();
        }

        /// @brief Removes the value with the given key from the map.
        /// @param key Index of the type to use as a key.
        inline void erase(std::type_index key)
        {
            mMap.erase(key);
        }

        /// @brief Removes the value with the given key from the map.
        /// @tparam K Type to use as a key.
        template <typename K>
        inline void erase()
        {
            this->erase(std::type_index(typeid(K)));
        }

        /// @brief Gets the number of values in the map.
        /// @return Number of values in the map.
        inline std::size_t size() const
        {
            return mMap.size();
        }

        /// @brief Gets an iterator to the beginning of the map.
        /// @return Iterator to the beginning of the map.
        inline auto begin() const
        {
            return mMap.begin();
        }

        /// @brief Gets an iterator to the end of the map.
        /// @return Iterator to the end of the map.
        inline auto end() const
        {
            return mMap.end();
        }

    private:
        std::unordered_map<std::type_index, V> mMap; ///< Map of values indexed by type index.
    };
} // namespace cubos::core::memory
