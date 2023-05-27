#pragma once

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ui
{
    /// ECS system which allows the user to inspect the entities and components in an ECS world.
    /// @param world World to show.
    /// @param context Optional context for serializing the world.
    void showWorld(const ecs::World& world, data::Context* context = nullptr);

    /// ECS system which allows the user to inspect and edit the entities and components in an ECS world.
    /// @param world World to edit.
    /// @param serContext Optional context for serializing the world.
    /// @param desContext Optional context for deserializing the world.
    void editWorld(ecs::World& world, data::Context* serContext = nullptr, data::Context* desContext = nullptr);
} // namespace cubos::core::ui
