#ifndef CUBOS_CORE_UI_ECS_HPP
#define CUBOS_CORE_UI_ECS_HPP

#include <cubos/core/ecs/debug.hpp>

namespace cubos::core::ui
{
    /// ECS system which allows the user to inspect the entities and components in an ECS world.
    /// @param debug Debug query.
    void showWorld(ecs::Debug debug);
} // namespace cubos::core::ui

#endif // CUBOS_CORE_UI_ECS_HPP
