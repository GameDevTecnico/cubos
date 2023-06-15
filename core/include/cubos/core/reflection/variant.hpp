/// @file
/// Defines the VariantType class, which is an implementation of Type for variant types.

#pragma once

#include <vector>

#include <cubos/core/reflection/type.hpp>

namespace cubos::core::reflection
{
    /// @brief Implementation of Type for variant types - types which can take the form of one or
    /// more other types.
    class VariantType : public Type
    {
    public:
        /// @brief Function type for getting the index of the active variant of an instance of this
        /// type.
        using Index = std::size_t (*)(const void*);

        /// @brief Represents a possible type of a variant type.
        class Variant final
        {
        public:
            /// @brief Function type for getting the address of a variant.
            using Address = uintptr_t (*)(const void*);

            /// @brief Function type for making this variant the active variant of an instance of
            /// the type, and constructing it in-place.
            using EmplaceDefault = void (*)(void*);

            /// @brief Constructs a new Variant object.
            /// @param type Type of this variant.
            /// @param address Function for getting the address of the variant.
            /// @param emplaceDefault Function to emplace the variant in an instance of the type.
            Variant(const Type& type, Address address, EmplaceDefault emplaceDefault);

            /// @brief Gets the type of this variant.
            /// @return Type of this variant.
            const Type& type() const;

            /// @brief Gets the address getter for this variant.
            /// @return Address getter for this variant.
            Address address() const;

            /// @brief Gets the emplace default function for this variant.
            /// @return Emplace default function for this variant.
            EmplaceDefault emplaceDefault() const;

            /// @name Getters to the this variant in an instance of the type.
            /// @brief Gets a pointer or reference to the this variant on the given instance of the
            /// type. Must be a valid instance of the type, otherwise, undefined behavior occurs.
            /// If the type does not hold this variant, the behavior is undefined. If an address
            /// getter is not provided, aborts.
            /// @param instance Pointer to the instance of the type.
            /// @return Pointer or reference to the field.
            /// @{
            const void* get(const void* instance) const;
            void* get(void* instance) const;

            /// @tparam T Type of the variant. Must match the type returned by type().
            template <typename T>
            const T& get(const void* instance) const
            {
                CUBOS_ASSERT(mType.is<T>(), "Expected variant of type {} but got {}", reflect<T>().name(),
                             mType.name());
                return *static_cast<const T*>(this->get(instance));
            }

            /// @tparam T Type of the field. Must match the type returned by type().
            template <typename T>
            T& get(void* variant) const
            {
                CUBOS_ASSERT(mType.is<T>(), "Expected variant of type {} but got {}", reflect<T>().name(),
                             mType.name());
                return *static_cast<T*>(this->get(variant));
            }
            /// @}

            /// @brief Makes this variant the active variant of an instance of the type,
            /// constructing a default value in-place. If an emplace default function is not
            /// provided, aborts.
            /// @param instance Pointer to the instance of the type.
            void emplaceDefault(void* instance) const;

        private:
            const Type& mType;
            Address mAddress;
            EmplaceDefault mEmplaceDefault;
        };

        /// @brief Builder for creating VariantType's.
        class Builder final : public Type::Builder
        {
        public:
            /// @brief Construct a new Builder object.
            /// @param type Type to build.
            Builder(VariantType& type);

            /// @brief Sets the function for getting the index of the active variant of an instance
            /// of this type.
            /// @param index Function for getting the index of the active variant of an instance of
            /// this type.
            /// @return This builder.
            Builder& index(Index index);

            /// @brief Sets the address getter for the next variant.
            /// @param address Function for getting the address of a variant.
            /// @return This builder.
            Builder& address(Variant::Address address);

            /// @brief Sets the emplace default function for the next variant.
            /// @param emplaceDefault Function for emplacing this variant in an instance of the
            /// type.
            /// @return This builder.
            Builder& emplaceDefault(Variant::EmplaceDefault emplaceDefault);

            /// @brief Adds a variant to the type. If not preceded by calls to address() and
            /// emplaceDefault(), those functions will be set to nullptr.
            /// @param type Type of the variant.
            /// @return This builder.
            Builder& variant(const Type& type);

            /// @brief Adds a variant to the variant.
            /// @tparam T Type of the variant.
            /// @return This builder.
            template <typename T>
            Builder& variant()
            {
                return this->variant(reflect<T>());
            }

        private:
            VariantType& mType;
            Variant::Address mAddress;
            Variant::EmplaceDefault mEmplaceDefault;
        };

        /// @brief Creates a new Builder for a VariantType with the given name.
        /// @param name Name of the type.
        /// @return Builder for the type.
        static Builder builder(std::string name);

        /// @brief Sets the function for getting the index of the active variant of an instance of
        /// this type.
        /// @param index Function for getting the index of the active variant of an instance of
        /// this type.
        /// @return This type.
        VariantType&& index(Index index) &&;

        /// @brief Gets the function for getting the index of the active variant of an instance of
        /// this type.
        /// @return Function for getting the index of the active variant of an instance of this
        /// type.
        Index index() const;

        /// @brief Gets all possible variants of the type.
        /// @return All possible variants of the type.
        const std::vector<Variant>& variants() const;

        /// @brief Gets the variant which has the given type.
        /// @param type Type of the variant to get.
        /// @return Variant which has the given type.
        const Variant& variant(const Type& type) const;

        /// @brief Gets the variant which has the given type.
        /// @tparam T Type of the variant to get.
        /// @return Value which has the given type.
        template <typename T>
        const Variant& variant() const
        {
            return this->variant(reflect<T>());
        }

        /// @brief Gets the current variant held by the given instance of this type. Aborts if the
        /// index function was not provided.
        /// @param instance Pointer to the instance of the type.
        /// @return Current variant held by the instance.
        const Variant& variant(const void* instance) const;

        /// @brief Gets the index of the current variant held by the given instance of this type.
        /// Aborts if the index function was not provided.
        /// @param instance Pointer to the instance of the type.
        /// @return Index of the current variant held by the instance.
        std::size_t index(const void* instance) const;

        virtual void accept(class TypeVisitor& visitor) const override;

    private:
        /// @brief Construct a new VariantType object.
        /// @param name Name of the type.
        VariantType(std::string name);

        std::vector<Variant> mVariants;
        Index mIndex;
    };
} // namespace cubos::core::reflection
