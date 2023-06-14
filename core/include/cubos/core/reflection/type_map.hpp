/// @file
/// @brief Defines the TypeMap class.

#pragma once

#include <unordered_map>

#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief A map of reflected types to data of a single type.
    /// @tparam T Type of the data.
    template <typename T>
    class TypeMap
    {
    public:
        /// @brief Inserts a new value into the map.
        /// @param type Type which will be used as the key.
        /// @param value Value to insert.
        /// @return True if the value was inserted, false if the key already existed.
        bool insert(const Type& type, T&& value)
        {
            return mMap.insert({&type, std::move(value)}).second;
        }

        /// @brief Inserts a new value into the map.
        /// @tparam U Type which will be used as the key.
        /// @param value Value to insert.
        /// @return True if the value was inserted, false if the key already existed.
        template <typename U>
        bool insert(T&& value)
        {
            return this->insert(reflect<U>(), std::move(value));
        }

        /// @brief Gets the value associated with the given type.
        /// @param type Type to get the value for.
        /// @return Value associated with the given type.
        /// @{
        T& at(const Type& type)
        {
            auto it = mMap.find(&type);
            CUBOS_ASSERT(it != mMap.end(), "Type not found in TypeMap");
            return it->second;
        }

        const T& at(const Type& type) const
        {
            auto it = mMap.find(&type);
            CUBOS_ASSERT(it != mMap.end(), "Type not found in TypeMap");
            return it->second;
        }
        /// @}

        /// @brief Gets the value associated with the given type.
        /// @tparam U Type to get the value for.
        /// @return Value associated with the given type.
        /// @{
        template <typename U>
        T& at()
        {
            return this->at(reflect<U>());
        }

        template <typename U>
        const T& at() const
        {
            return this->at(reflect<U>());
        }
        /// @}

        /// @brief Checks if the given type is in the map.
        /// @param type Type to check for.
        /// @return True if the type is in the map, false otherwise.
        bool contains(const Type& type) const
        {
            return mMap.find(&type) != mMap.end();
        }

        /// @brief Checks if the given type is in the map.
        /// @tparam U Type to check for.
        /// @return True if the type is in the map, false otherwise.
        template <typename U>
        bool contains() const
        {
            return this->contains(reflect<U>());
        }

        /// @brief Gets an iterator to the beginning of the map.
        /// @return Iterator to the beginning of the map.
        /// @{
        auto begin()
        {
            return mMap.begin();
        }

        auto begin() const
        {
            return mMap.begin();
        }
        /// @}

        /// @brief Gets an iterator to the end of the map.
        /// @return Iterator to the end of the map.
        /// @{
        auto end()
        {
            return mMap.end();
        }

        auto end() const
        {
            return mMap.end();
        }
        /// @}

    private:
        std::unordered_map<const Type*, T> mMap;
    };
} // namespace cubos::core::reflection