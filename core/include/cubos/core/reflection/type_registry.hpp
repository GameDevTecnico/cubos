/// @file
/// @brief Class @ref cubos::core::reflection::TypeRegistry.
/// @ingroup core-reflection

#pragma once

#include <string>

#include <cubos/core/memory/unordered_bimap.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Stores a set of types which can be accessed by name.
    /// @ingroup core-reflection
    class TypeRegistry final
    {
    public:
        using Iterator = typename memory::UnorderedBimap<const Type*, std::string>::Iterator;

        /// @brief Registers the given type.
        ///
        /// Does nothing if the type is already registered.
        /// Aborts if a different type with the same name is already registered.
        ///
        /// @param type Type to register.
        void insert(const Type& type);

        /// @brief Calls insert for each type in the given type registry.
        /// @param other Type registry to copy types from.
        void insert(const TypeRegistry& other);

        /// @copydoc insert(const Type&)
        /// @tparam T Type to register.
        template <typename T>
        void insert()
        {
            this->insert(reflect<T>());
        }

        /// @brief Checks if the given type is registered.
        /// @param type Type to check.
        /// @return Whether the given type is registered.
        bool contains(const Type& type) const;

        /// @copydoc contains(const Type&)
        /// @tparam T Type to check.
        template <typename T>
        bool contains() const
        {
            return this->contains(reflect<T>());
        }

        /// @brief Checks if a type with the given name is registered.
        /// @param name Name of the type.
        /// @return Whether a type with the given name is registered.
        bool contains(const std::string& name) const;

        /// @brief Returns the type with the given name.
        ///
        /// Aborts if @ref contains(const std::string&) returns false.
        ///
        /// @param name Name of the type.
        /// @return Type with the given name.
        const Type& at(const std::string& name) const;

        /// @brief Returns the number of registered types.
        /// @return Number of registered types.
        std::size_t size() const;

        /// @brief Gets an iterator to the beginning of the registry.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator to the end of the registry.
        /// @return Iterator.
        Iterator end() const;

    private:
        memory::UnorderedBimap<const Type*, std::string> mTypes{};
    };
} // namespace cubos::core::reflection
