/// @file
/// @brief Class @ref cubos::core::ecs::TypeBuilder.
/// @ingroup core-ecs

#pragma once

#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::core::ecs
{
    /// @brief Trait used to identify symmetric relations.
    ///
    /// Symmetric relations are relations where the order of the entities does not matter.
    ///
    /// @ingroup core-ecs
    struct SymmetricTrait
    {
    };

    /// @brief Trait used to identify tree relations.
    ///
    /// Tree relations are relations which form trees. Each entity must have at most one target entity for a given tree
    /// relation type. These can be used to represent parent-child relationships.
    ///
    /// @ingroup core-ecs
    struct TreeTrait
    {
    };

    /// @brief Trait used to identify types which are ephemeral and should not be persisted.
    /// @ingroup core-ecs
    struct EphemeralTrait
    {
    };

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

        /// @brief Makes the type symmetric. Only used by relation types.
        /// @return Builder.
        TypeBuilder&& symmetric() &&
        {
            mType.with(SymmetricTrait{});
            return std::move(*this);
        }

        /// @brief Makes the type a tree relation. Only used by relation types.
        /// @return Builder.
        TypeBuilder&& tree() &&
        {
            mType.with(TreeTrait{});
            return std::move(*this);
        }

        /// @brief Makes the type ephemeral.
        /// @return Builder.
        TypeBuilder&& ephemeral() &&
        {
            mType.with(EphemeralTrait{});
            return std::move(*this);
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
