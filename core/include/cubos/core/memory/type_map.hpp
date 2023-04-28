#ifndef CUBOS_CORE_MEMORY_TYPE_MAP_HPP
#define CUBOS_CORE_MEMORY_TYPE_MAP_HPP

#include <optional>
#include <typeindex>
#include <unordered_map>

namespace cubos::core::memory
{
    /// A map that stores values of a specific type, using types as keys.
    /// @tparam V The type of the values to store.
    template <typename V>
    class TypeMap
    {
    public:
        TypeMap() = default;
        TypeMap(TypeMap&&) noexcept = default;
        ~TypeMap() = default;

        /// @param key The key to identify the value.
        /// @param value The value to store.
        inline void set(std::type_index key, V value)
        {
            mMap.emplace(key, std::move(value));
        }

        /// @param key The key to identify the value.
        /// @returns The value stored in the map, or null if no value is stored.
        inline V* at(std::type_index key)
        {
            auto it = mMap.find(key);
            if (it == mMap.end())
            {
                return nullptr;
            }
            return &it->second;
        }

        /// @param key The key to identify the value.
        /// @returns The value stored in the map, or null if no value is stored.
        inline const V* at(std::type_index key) const
        {
            auto it = mMap.find(key);
            if (it == mMap.end())
            {
                return nullptr;
            }
            return &it->second;
        }

        /// @tparam K The type of the key.
        /// @param value The value to store.
        template <typename K>
        inline void set(V value)
        {
            this->set(std::type_index(typeid(K)), std::move(value));
        }

        /// @tparam K The type of the key.
        /// @returns The value stored in the map, or null if no value is stored.
        template <typename K>
        inline V* at()
        {
            return this->at(std::type_index(typeid(K)));
        }

        /// @tparam K The type of the key.
        /// @returns The value stored in the map, or null if no value is stored.
        template <typename K>
        inline const V* at() const
        {
            return this->at(std::type_index(typeid(K)));
        }

        /// Removes all elements from the map.
        inline void clear()
        {
            mMap.clear();
        }

        /// Removes the element with the given key from the map.
        /// @param key The key to identify the value.
        inline void erase(std::type_index key)
        {
            mMap.erase(key);
        }

        /// Removes the element with the given key from the map.
        /// @tparam K The key to identify the value.
        template <typename K>
        inline void erase()
        {
            this->erase(std::type_index(typeid(K)));
        }

        /// @returns The number of elements in the map.
        inline std::size_t size() const
        {
            return mMap.size();
        }

        /// @returns Iterator to the beginning of the map.
        inline auto begin() const
        {
            return mMap.begin();
        }

        /// @returns Iterator to the end of the map.
        inline auto end() const
        {
            return mMap.end();
        }

    private:
        std::unordered_map<std::type_index, V> mMap; ///< The map of values.
    };
} // namespace cubos::core::memory

#endif // CUBOS_CORE_MEMORY_TYPE_MAP_HPP
