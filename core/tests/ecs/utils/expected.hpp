#pragma once

#include <unordered_map>
#include <unordered_set>

#include <cubos/core/ecs/world.hpp>

namespace test::ecs
{
    using cubos::core::ecs::Entity;
    using cubos::core::ecs::EntityHash;
    using cubos::core::ecs::World;
    using cubos::core::reflection::Type;

    struct ExpectedInteger
    {
        int value;

        // Dynamically creates a new reflection type for this struct.
        static Type& createType(std::string name);
    };

    struct ExpectedEntity
    {
        // Holds the components which this entity has and their expected values.
        std::unordered_map<const Type*, int> components;

        // Holds the relations which this entity is a part of as the 'to' entity, and their expected values.
        std::unordered_map<const Type*, std::unordered_map<Entity, int, EntityHash>> incoming;

        // Holds the relations which this entity is a part of as the 'from' entity, and their expected values.
        std::unordered_map<const Type*, std::unordered_map<Entity, int, EntityHash>> outgoing;
    };

    struct ExpectedWorld
    {
        std::unordered_set<Type*> componentTypes;
        std::unordered_set<Type*> relationTypes;
        std::unordered_map<Entity, ExpectedEntity, EntityHash> aliveEntities;
        std::unordered_set<Entity, EntityHash> deadEntities;

        ~ExpectedWorld();

        const Type& registerComponent(std::string name);
        const Type& registerRelation(std::string name);
        const Type& registerSymmetricRelation(std::string name);
        const Type& registerTreeRelation(std::string name);

        // Checks if the given world types match the expected types.
        void testTypes(World& world);

        // Check if the given world entity matches its expected state.
        void testEntity(World& world, Entity entity);

        // Checks if the given world entities match the expected entities.
        void testEntities(World& world);
    };
} // namespace test::ecs
