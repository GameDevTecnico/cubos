/// @file
/// @brief Class @ref cubos::core::ecs::TypeBuilder.
/// @ingroup core-ecs

#pragma once

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::ecs
{
    /// @brief Builder for @ref reflection::Type objects which represent ECS types.
    ///
    /// Used to reduce the amount of boilerplate code required to define a ECS types.
    /// Automatically adds the @ref reflection::ConstructibleTrait and @ref reflection::FieldsTrait.
    /// The type @p T must be default-constructible, copy-constructible and move-constructible.
    ///
    /// @tparam T Relation type.
    /// @ingroup core-ecs
    template <typename T>
    class TypeBuilder
    {
    public:
        /// @brief Constructs.
        /// @param name Type name, including namespace.
        TypeBuilder(std::string name)
            : mType(reflection::Type::create(std::move(name)))
        {
            mType.with(reflection::ConstructibleTrait::typed<T>().withBasicConstructors().build());
        }

        /// @brief Adds a field to the type.
        /// @tparam F Field type.
        /// @param name Field name.
        /// @param pointer Field pointer.
        /// @return Builder.
        template <typename F>
        TypeBuilder&& withField(std::string name, F T::*pointer) &&
        {
            mFields.addField(std::move(name), pointer);
            return std::move(*this);
        }

        /// @brief Builds the type.
        /// @return Relation type.
        reflection::Type& build() &&
        {
            mType.with(std::move(mFields));
            return mType;
        }

    private:
        reflection::Type& mType;
        reflection::FieldsTrait mFields;
    };
} // namespace cubos::core::ecs
