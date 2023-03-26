#pragma once

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ui
{
    /// ECS system which allows the user to inspect the entities and components in an ECS world.
    /// @param world World to show.
    /// @param context Context for serializing the world.
    void showWorld(const ecs::World& world, data::Context& context);

    /// ECS system which allows the user to inspect and edit the entities and components in an ECS world.
    /// @param world World to edit.
    /// @param serContext Context for serializing the world.
    /// @param desContext Context for deserializing the world.
    void editWorld(ecs::World& world, data::Context& serContext, data::Context& desContext);
} // namespace cubos::core::ui
