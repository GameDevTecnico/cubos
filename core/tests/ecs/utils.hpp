/// @file
/// Contains utilities used by ECS-related tests which reduce the boilerplate.

#pragma once

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include "../utils.hpp"

/// A component which stores a single integer.
struct [[cubos::component("integer")]] IntegerComponent
{
    CUBOS_REFLECT;

    int value;
};

/// A component which references another entity.
struct [[cubos::component("parent")]] ParentComponent
{
    CUBOS_REFLECT;

    cubos::core::ecs::Entity id;
};

/// A component used to test if components are destructed properly.
struct [[cubos::component("detect_destructor")]] DetectDestructorComponent
{
    CUBOS_REFLECT;

    [[cubos::ignore]] DetectDestructor detect;
};

/// Adds the utility components to a world.
inline void setupWorld(cubos::core::ecs::World& world)
{
    world.registerComponent<IntegerComponent>();
    world.registerComponent<ParentComponent>();
    world.registerComponent<DetectDestructorComponent>();
}
