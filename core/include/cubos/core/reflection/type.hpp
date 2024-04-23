/// @file
/// @brief Class @ref cubos::core::reflection::Type.
/// @ingroup core-reflection

#pragma once

#include <cstdint>
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
    class CUBOS_CORE_API Type final
    {
    public:
        /// @name Forbid any kind of copying.
        /// @{
        Type(const Type&) = delete;
        Type& operator=(const Type&) = delete;
        /// @}

        /// @brief Constructs with a type the given name.
        /// @param name Name of the type.
        /// @return Reference to the type.
        static Type& create(std::string name);

        /// @brief Constructs an unnamed type with the given identifier.
        ///
        /// The type is also marked as not being implemented.
        ///
        /// @param id Type identifier.
        /// @return Reference to the type.
        static Type& unnamed(unsigned long long id);

        /// @brief Destroys the given type.
        /// @param type Type to destroy.
        static void destroy(Type& type);

        /// @brief Returns the name of the type.
        /// @return Name of the type.
        const std::string& name() const;

        /// @brief Returns the name of the type without any namespaces.
        /// @return Name of the type without any namespaces.
        const std::string& shortName() const;

        /// @brief Checks if this type represents the type @p T.
        /// @tparam T Type to check.
        /// @return Whether this type represents the type @p T.
        template <typename T>
        bool is() const
        {
            return this == &reflect<T>();
        }

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
            return this->with(reflect<T>(), new T(std::move(trait)),
                              [](void* trait) { delete static_cast<T*>(trait); });
        }

        /// @brief Adds the given trait to the type.
        /// @param id Trait type.
        /// @param trait Allocated trait value.
        /// @param deleter Used to delete the trait when the type is destroyed.
        /// @return Reference to this type, for chaining.
        Type& with(const Type& type, void* trait, void (*deleter)(void*));

        /// @brief Returns whether the type has the given trait.
        /// @tparam T %Trait type.
        /// @return Whether the type has the given trait.
        template <typename T>
        bool has() const
        {
            return this->has(reflect<T>());
        }

        /// @brief Returns whether the type has the given trait.
        /// @param id Identifies the type of the trait.
        /// @return Whether the type has the given trait.
        bool has(const Type& type) const;

        /// @brief Returns the given trait of the type.
        ///
        /// Aborts if the type does not have the given trait.
        ///
        /// @tparam T %Trait type.
        /// @return Reference to the trait.
        template <typename T>
        const T& get() const
        {
            return *static_cast<const T*>(this->get(reflect<T>()));
        }

        /// @brief Returns the given trait of the type.
        ///
        /// Aborts if the type does not have the given trait.
        ///
        /// @param type Trait type.
        /// @return Pointer to the trait.
        const void* get(const Type& type) const;

        /// @brief Equality operator.
        /// @param other Type object to compare.
        /// @return Whether the objects have the same address, which indicates equality.
        bool operator==(const Type& other) const;

        /// @brief Checks whether the type had reflection implemented for it.
        bool implemented() const;

    private:
        ~Type();

        /// @brief Constructs with the given name.
        /// @param name Name of the type.
        explicit Type(std::string name);

        /// @brief %Trait entry in the type.
        struct Trait
        {
            const Type& type;       ///< Trait type.
            void* value;            ///< Value, allocated on the heap.
            void (*deleter)(void*); ///< Deleter function.
        };

        std::string mName;
        std::string mShortName;
        bool mImplemented{true};
        std::vector<Trait> mTraits;
    };
} // namespace cubos::core::reflection
