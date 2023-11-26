/// @file
/// Contains utilities used by ECS-related tests which reduce the boilerplate.

#pragma once

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include "../utils.hpp"

/// A component which stores a single integer.
struct IntegerComponent
{
    CUBOS_REFLECT;

    int value;
};

/// A component which references another entity.
struct ParentComponent
{
    CUBOS_REFLECT;

    cubos::core::ecs::Entity id;
};

/// A component used to test if components are destructed properly.
struct DetectDestructorComponent
{
    CUBOS_REFLECT;

    DetectDestructor detect;
};

struct EntityArrayComponent
{
    CUBOS_REFLECT;

    std::vector<cubos::core::ecs::Entity> vec;
};

struct EntityDictionaryComponent
{
    CUBOS_REFLECT;

    std::unordered_map<int, cubos::core::ecs::Entity> map;
};

/// Adds the utility components to a world.
inline void setupWorld(cubos::core::ecs::World& world)
{
    world.registerComponent<IntegerComponent>();
    world.registerComponent<ParentComponent>();
    world.registerComponent<DetectDestructorComponent>();
    world.registerComponent<EntityArrayComponent>();
    world.registerComponent<EntityDictionaryComponent>();
}
