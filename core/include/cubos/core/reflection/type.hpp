/// @file
/// @brief Class @ref cubos::core::reflection::Type.
/// @ingroup core-reflection

#pragma once

#include <string>
#include <vector>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Describes a reflected type.
    ///
    /// Holds the name of a type and the traits associated with it. Traits can be of any type,
    /// which means you can define your own custom traits.
    ///
    /// @see This class holds the data returned by the @ref reflect() function.
    /// @ingroup core-reflection
    class Type final
    {
    public:
        Type(const Type&) = delete;
        Type(Type&&) = delete;

        /// @brief Constructs with a type the given name.
        /// @param name Name of the type.
        /// @return Reference to the type.
        static Type& create(std::string name);

        /// @brief Destroys the given type.
        /// @param type Type to destroy.
        static void destroy(Type& type);

        /// @brief Returns the name of the type.
        /// @return Name of the type.
        const std::string& name() const;

        /// @brief Adds the given trait to the type.
        ///
        /// Aborts if the trait is already present in the type.
        ///
        /// @tparam T %Trait type.
        /// @param trait %Trait value.
        /// @return Reference to this type, for chaining.
        template <typename T>
        Type& with(T trait)
        {
            return this->with(Type::id<T>(), new T(trait), [](void* trait) { delete static_cast<T*>(trait); });
        }

        /// @brief Returns whether the type has the given trait.
        /// @tparam T %Trait type.
        /// @return Whether the type has the given trait.
        template <typename T>
        bool has() const
        {
            return this->has(Type::id<T>());
        }

        /// @brief Returns the given trait of the type.
        ///
        /// Aborts if the type does not have the given trait.
        ///
        /// @tparam T %Trait type.
        /// @return Reference to the trait.
        template <typename T>
        const T& get() const
        {
            return *static_cast<const T*>(this->get(Type::id<T>()));
        }

    private:
        ~Type();

        /// @brief Constructs with the given name.
        /// @param name Name of the type.
        explicit Type(std::string name);

        /// @brief Adds the given trait to the type.
        /// @param id Identifies the type of the trait.
        /// @param trait Allocated trait value.
        /// @param deleter Used to delete the trait when the type is destroyed.
        /// @return Reference to this type, for chaining.
        Type& with(uintptr_t id, void* trait, void (*deleter)(void*));

        /// @brief Returns whether the type has the given trait.
        /// @param id Identifies the type of the trait.
        /// @return Whether the type has the given trait.
        bool has(uintptr_t id) const;

        /// @brief Returns the given trait of the type.
        ///
        /// Aborts if the type does not have the given trait.
        ///
        /// @param id Identifies the type of the trait.
        /// @return Pointer to the trait.
        const void* get(uintptr_t id) const;

        /// @brief Gets an unique identifier for the given trait type.
        /// @note This function is used as an alternative to `std::type_index`, which would require
        /// including the `<typeindex>` header.
        /// @tparam T %Trait type.
        /// @return Unique identifier for the given trait type.
        template <typename T>
        static uintptr_t id()
        {
            // This variable is unused, but since there is one for each type, its address is
            // guaranteed to be unique for each type.
            static const bool var = false;
            return reinterpret_cast<uintptr_t>(&var);
        }

        /// @brief %Trait entry in the type.
        struct Trait
        {
            uintptr_t id;           ///< Type identifier.
            void* value;            ///< Value, allocated on the heap.
            void (*deleter)(void*); ///< Deleter function.
        };

        std::string mName;
        std::vector<Trait> mTraits;
    };
} // namespace cubos::core::reflection
