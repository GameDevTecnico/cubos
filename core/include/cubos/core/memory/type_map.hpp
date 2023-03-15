#ifndef CUBOS_CORE_MEMORY_TYPE_MAP_HPP
#define CUBOS_CORE_MEMORY_TYPE_MAP_HPP

#include <unordered_map>
#include <typeindex>
#include <optional>

namespace cubos::core::memory
{
    /// A map that stores values of a specific type, using types as keys.
    /// @tparam V The type of the values to store.
    template <typename V>
    class TypeMap
    {
    public:
        TypeMap() = default;
        ~TypeMap() = default;

        /// @param key The key to identify the value.
        /// @param value The value to store.
        inline void set(std::type_index key, V&& value)
        {
            this->map.emplace(key, std::move(value));
        }

        /// @param key The key to identify the value.
        /// @returns The value stored in the map, if any.
        inline std::optional<V&> at(std::type_index key)
        {
            auto it = this->map.find(key);
            if (it == this->map.end())
                return std::nullopt;
            return it->second;
        }

        /// @param key The key to identify the value.
        /// @returns The value stored in the map, if any.
        inline std::optional<const V&> at(std::type_index key) const
        {
            auto it = this->map.find(key);
            if (it == this->map.end())
                return std::nullopt;
            return it->second;
        }

        /// @tparam K The type of the key.
        /// @param value The value to store.
        template <typename K>
        inline void set(V&& value)
        {
            this->set(std::type_index(typeid(K)), std::move(value));
        }

        /// @tparam K The type of the key.
        /// @returns The value stored in the map, if any.
        template <typename K>
        inline std::optional<V&> at()
        {
            return this->at(std::type_index(typeid(K)));
        }

        /// @tparam K The type of the key.
        /// @returns The value stored in the map, if any.
        template <typename K>
        inline std::optional<const V&> at() const
        {
            return this->at(std::type_index(typeid(K)));
        }

        /// Removes all elements from the map.
        inline void clear()
        {
            this->map.clear();
        }

        /// Removes the element with the given key from the map.
        /// @param key The key to identify the value.
        inline void erase(std::type_index key)
        {
            this->map.erase(key);
        }

        /// Removes the element with the given key from the map.
        /// @tparam K The key to identify the value.
        template <typename K>
        inline void erase()
        {
            this->erase(std::type_index(typeid(K)));
        }

        /// @returns The number of elements in the map.
        inline size_t size() const
        {
            return this->map.size();
        }

    private:
        std::unordered_map<std::type_index, V> map; ///< The map of values.
    };
} // namespace cubos::core::memory

#endif // CUBOS_CORE_MEMORY_TYPE_MAP_HPP
