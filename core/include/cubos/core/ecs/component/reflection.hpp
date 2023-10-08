/// @file
/// @brief Class @ref cubos::core::ecs::ComponentTypeBuilder.
/// @ingroup core-ecs-component

#pragma once

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::ecs
{
    /// @brief Builder for @ref reflection::Type objects which represent component types.
    ///
    /// Used to reduce the amount of boilerplate code required to define a component type.
    /// Automatically adds the @ref reflection::ConstructibleTrait and @ref reflection::FieldsTrait.
    /// The type @p T must be default-constructible, copy-constructible and move-constructible.
    ///
    /// @tparam T Component type.
    template <typename T>
    class ComponentTypeBuilder
    {
    public:
        /// @brief Constructs.
        /// @param name Component type name, including namespace.
        ComponentTypeBuilder(std::string name)
            : mType(reflection::Type::create(std::move(name)))
        {
            mType.with(reflection::ConstructibleTrait::typed<T>().withBasicConstructors().build());
        }

        /// @brief Adds a field to the component type.
        /// @tparam F Field type.
        /// @param name Field name.
        /// @param pointer Field pointer.
        /// @return Builder.
        template <typename F>
        ComponentTypeBuilder&& withField(std::string name, F T::*pointer) &&
        {
            mFields.addField(std::move(name), pointer);
            return std::move(*this);
        }

        /// @brief Builds the component type.
        /// @return Component type.
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
