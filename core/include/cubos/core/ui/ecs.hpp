/// @file
/// @brief Functions @ref cubos::core::ui::showWorld and @ref cubos::core::ui::editWorld.
/// @ingroup core-ui

#pragma once

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ui
{
    /// @brief Adds a ImGui window which allows the user to inspect the entities and components in
    /// a @ref ecs::World.
    /// @param world World to show.
    /// @param context Optional context for serializing the world.
    /// @ingroup core-ui
    void showWorld(const ecs::World& world, data::Context* context = nullptr);

    /// @brief Adds a ImGui window which allows the user to edit the entities and components in a
    /// @ref ecs::World.
    /// @param world World to edit.
    /// @param serContext Optional context for serializing the world.
    /// @param desContext Optional context for deserializing the world.
    /// @ingroup core-ui
    void editWorld(ecs::World& world, data::Context* serContext = nullptr, data::Context* desContext = nullptr);
} // namespace cubos::core::ui
