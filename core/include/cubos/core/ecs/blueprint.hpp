/// @file
/// @brief Class @ref cubos::core::ecs::Blueprint.
/// @ingroup core-ecs

#pragma once

#include <string>
#include <unordered_map>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/memory/type_map.hpp>
#include <cubos/core/memory/unordered_bimap.hpp>

namespace cubos::core::ecs
{
    /// @brief Collection of entities and their respective components.
    ///
    /// Blueprints are in a way the 'Prefab' of @b CUBOS. They act as tiny @ref World which can
    /// then be spawned into an actual @ref World, as many times as needed.
    ///
    /// When a blueprint is spawned, all of its components are scanned using the @ref
    /// core-reflection system for any references to other entities in the blueprint. These
    /// references are then replaced with the actual spawned entities. This has the side effect
    /// that if you do not expose an @ref Entity field to the @ref core-reflection system, it will
    /// not be replaced and thus continue referencing the original entity in the blueprint.
    ///
    /// @ingroup core-ecs
    class Blueprint final
    {
    public:
        /// @brief Function used by @ref instantiate to create entities.
        /// @param userData User data passed into @ref instantiate.
        /// @param name Entity name.
        /// @return Instantiated entity.
        using Create = Entity (*)(void* userData, std::string name);

        /// @brief Function used by @ref instantiate to add components to entities.
        /// @param userData User data passed into @ref instantiate.
        /// @param entity Entity.
        /// @param component Component.
        using Add = void (*)(void* userData, Entity entity, memory::AnyValue component);

        /// @brief Constructs.
        Blueprint();

        /// @brief Move constructs.
        /// @param other Blueprint to move from.
        Blueprint(Blueprint&& other);

        /// @brief Creates a new entity in the blueprint and returns it.
        ///
        /// An entity with the same name must not exist. The name must be @ref validEntityName
        /// "valid".
        ///
        /// @warning The returned entity is only valid inside the blueprint itself.
        /// @param name Entity name.
        /// @return Entity.
        Entity create(std::string name);

        /// @brief Adds a component to an entity. Overwrites the existing component, if there's any.
        /// @param entity Entity.
        /// @param component Component to move.
        void add(Entity entity, memory::AnyValue component);

        /// @brief Adds components to an entity. Overwrites the existing components, if there's any.
        /// @tparam Ts Component types.
        /// @param entity Entity.
        /// @param components Components to move.
        template <reflection::Reflectable... Ts>
        void add(Entity entity, Ts... components)
        {
            ([&]() { this->add(entity, memory::AnyValue::moveConstruct(reflection::reflect<Ts>(), &components)); }(),
             ...);
        }

        /// @brief Merges another blueprint into this one.
        ///
        /// Entities in the other blueprint will have their names prefixed with the specified
        /// string.
        ///
        /// @param prefix Name to prefix with the merged blueprint.
        /// @param other Blueprint to merge.
        void merge(const std::string& prefix, const Blueprint& other);

        /// @brief Clears the blueprint.
        void clear();

        /// @brief Returns a bimap which maps entities to their names.
        /// @return Bimap of entities to names.
        const memory::UnorderedBimap<Entity, std::string, EntityHash>& bimap() const;

        /// @brief Instantiates the blueprint by calling the given functions.
        /// @param userData User data to pass into the functions.
        /// @param create Function used to create entities.
        /// @param add Function used to add components to entities.
        void instantiate(void* userData, Create create, Add add) const;

        /// @brief Instantiates the blueprint by calling the given functors.
        /// @tparam C Create functor type.
        /// @tparam A Add functor type.
        /// @param create Functor used to create entities.
        /// @param add Functor used to add components to entities.
        template <typename C, typename A>
        void instantiate(C create, A add) const
        {
            auto functors = std::make_pair(create, add);

            Create createFunc = [](void* userData, std::string name) -> Entity {
                return static_cast<decltype(functors)*>(userData)->first(name);
            };

            Add addFunc = [](void* userData, Entity entity, memory::AnyValue component) {
                return static_cast<decltype(functors)*>(userData)->second(entity, std::move(component));
            };

            // We pass the functors pair using the userData argument. We could use std::function
            // here and pass them directly, but that would mean unnecessary heap allocations and an
            // extra large include on the header.
            this->instantiate(&functors, createFunc, addFunc);
        }

        /// @brief Checks if the given name is a valid entity name.
        ///
        /// Entity names must contain only lowercase alphanumerical characters and hyphens.
        ///
        /// @return Whether the name is valid.
        static bool validEntityName(const std::string& name);

    private:
        /// @brief Maps entities to their names.
        memory::UnorderedBimap<Entity, std::string, EntityHash> mBimap;

        /// @brief Maps component types to maps of entities to the component values.
        memory::TypeMap<std::unordered_map<Entity, memory::AnyValue, EntityHash>> mComponents;
    };
} // namespace cubos::core::ecs
