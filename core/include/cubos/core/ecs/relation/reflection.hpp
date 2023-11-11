/// @file
/// @brief Class @ref cubos::core::ecs::RelationTypeBuilder.
/// @ingroup core-ecs-relation

#pragma once

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::ecs
{
    /// @brief Builder for @ref reflection::Type objects which represent relation types.
    ///
    /// Used to reduce the amount of boilerplate code required to define a relation type.
    /// Automatically adds the @ref reflection::ConstructibleTrait and @ref reflection::FieldsTrait.
    /// The type @p T must be default-constructible, copy-constructible and move-constructible.
    ///
    /// @tparam T Relation type.
    template <typename T>
    class RelationTypeBuilder
    {
    public:
        /// @brief Constructs.
        /// @param name Relation type name, including namespace.
        RelationTypeBuilder(std::string name)
            : mType(reflection::Type::create(std::move(name)))
        {
            mType.with(reflection::ConstructibleTrait::typed<T>().withBasicConstructors().build());
        }

        /// @brief Adds a field to the relation type.
        /// @tparam F Field type.
        /// @param name Field name.
        /// @param pointer Field pointer.
        /// @return Builder.
        template <typename F>
        RelationTypeBuilder&& withField(std::string name, F T::*pointer) &&
        {
            mFields.addField(std::move(name), pointer);
            return std::move(*this);
        }

        /// @brief Builds the relation type.
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
