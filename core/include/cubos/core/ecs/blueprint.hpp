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
    /// @brief Converts entities in a value to their respective new entities. If an entity is not found in the map, it
    /// is left unchanged.
    /// @param map Map of old entities to new entities.
    /// @param type Value type.
    /// @param value Value.
    CUBOS_CORE_API void convertEntities(const std::unordered_map<Entity, Entity, EntityHash>& map,
                                        const reflection::Type& type, void* value);

    /// @brief Collection of entities and their respective components and relations.
    ///
    /// Blueprints are in a way the 'Prefab' of @b CUBOS. They act as a tiny @ref World which can
    /// then be spawned into an actual @ref World, as many times as needed.
    ///
    /// When a blueprint is spawned, all of its components and relations are scanned using the @ref
    /// core-reflection system for any references to other entities in the blueprint. These
    /// references are then replaced with the actual spawned entities. This has the side effect
    /// that if you do not expose an @ref Entity field to the @ref core-reflection system, it will
    /// not be replaced and thus continue referencing the original entity in the blueprint.
    ///
    /// @ingroup core-ecs
    class CUBOS_CORE_API Blueprint final
    {
    public:
        /// @brief Function used by @ref instantiate to create entities.
        using Create = Entity (*)(void* userData, std::string name);

        /// @brief Function used by @ref instantiate to add components to entities.
        using Add = void (*)(void* userData, Entity entity, memory::AnyValue component);

        /// @brief Function used by @ref instantiate to add relations to entities.
        using Relate = void (*)(void* userData, Entity fromEntity, Entity toEntity, memory::AnyValue relation);

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
        template <typename... Ts>
        void add(Entity entity, Ts... components)
        {
            ([&]() { this->add(entity, memory::AnyValue::moveConstruct(reflection::reflect<Ts>(), &components)); }(),
             ...);
        }

        /// @brief Adds a relation between two entities. Overwrites the existing relation, if there's any.
        /// @param fromEntity From entity.
        /// @param toEntity To entity.
        /// @param relation Relation to move.
        void relate(Entity fromEntity, Entity toEntity, memory::AnyValue relation);

        /// @brief Adds a relation between two entities. Overwrites the existing relation, if there's any.
        /// @tparam T Relation type.
        /// @param fromEntity From entity.
        /// @param toEntity To entity.
        /// @param relation Relation to move.
        template <typename T>
        void relate(Entity fromEntity, Entity toEntity, T relation)
        {
            this->relate(fromEntity, toEntity, memory::AnyValue::moveConstruct(reflection::reflect<T>(), &relation));
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
        /// @param relate Function used to add relations to entities.
        /// @param withName Whether to add the 'Name' component to instantiated entities.
        void instantiate(void* userData, Create create, Add add, Relate relate, bool withName) const;

        /// @brief Instantiates the blueprint by calling the given functors.
        /// @tparam C Create functor type.
        /// @tparam A Add functor type.
        /// @tparam R Relate functor type.
        /// @param create Functor used to create entities.
        /// @param add Functor used to add components to entities.
        /// @param relate Functor used to add relations to entities.
        /// @param withName Whether to use the entity names from the blueprint.
        template <typename C, typename A, typename R>
        void instantiate(C create, A add, R relate, bool withName) const
        {
            struct Functors
            {
                C create;
                A add;
                R relate;
            };

            Functors functors{create, add, relate};

            Create createFunc = [](void* userData, std::string name) -> Entity {
                return static_cast<Functors*>(userData)->create(name);
            };

            Add addFunc = [](void* userData, Entity entity, memory::AnyValue component) {
                return static_cast<Functors*>(userData)->add(entity, std::move(component));
            };

            Relate relateFunc = [](void* userData, Entity fromEntity, Entity toEntity, memory::AnyValue relation) {
                return static_cast<Functors*>(userData)->relate(fromEntity, toEntity, std::move(relation));
            };

            // We pass the functors pair using the userData argument. We could use std::function
            // here and pass them directly, but that would mean unnecessary heap allocations and an
            // extra large include on the header.
            this->instantiate(&functors, createFunc, addFunc, relateFunc, withName);
        }

        /// @brief Checks if the given name is a valid entity name.
        ///
        /// Entity names must contain only lowercase alphanumerical characters and hyphens.
        ///
        /// @return Whether the name is valid.
        static bool validEntityName(const std::string& name);

        template <typename T>
        using EntityMap = std::unordered_map<Entity, T, EntityHash>;

        /// @brief Gets the map relating entities to their name
        /// @return Bimap relating entities and names
        memory::UnorderedBimap<Entity, std::string, EntityHash> entities() const
        {
            return mBimap;
        }

        /// @brief Gets the map relating types of components to maps of entities to the component values.
        /// @return TypeMap of an EntityMap to component values
        memory::TypeMap<EntityMap<memory::AnyValue>> components() const
        {
            return mComponents;
        }

        /// @brief Gets the map relating types of relations to maps of entities to maps of entities to the component
        /// values.
        /// @return TypeMap of an EntityMap to another EntityMap to component values
        memory::TypeMap<EntityMap<EntityMap<memory::AnyValue>>> relations() const
        {
            return mRelations;
        }

    private:
        /// @brief Maps entities to their names.
        memory::UnorderedBimap<Entity, std::string, EntityHash> mBimap;

        /// @brief Maps component types to maps of entities to the component values.
        memory::TypeMap<EntityMap<memory::AnyValue>> mComponents;

        /// @brief Maps component types to maps of entities to maps of entities to the relation values.
        memory::TypeMap<EntityMap<EntityMap<memory::AnyValue>>> mRelations;
    };
} // namespace cubos::core::ecs
