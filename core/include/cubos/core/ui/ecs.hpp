#ifndef CUBOS_CORE_UI_ECS_HPP
#define CUBOS_CORE_UI_ECS_HPP

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ui
{
    /// ECS system which allows the user to inspect the entities and components in an ECS world.
    /// @param world World to show.
    /// @param handleCtx Context for serializing handles.
    void showWorld(const ecs::World& world, data::Handle::SerContext handleCtx);

    /// ECS system which allows the user to inspect and edit the entities and components in an ECS world.
    /// @param world World to edit.
    /// @param handleSerCtx Context for serializing handles.
    /// @param handleDesCtx Context for deserializing handles.
    void editWorld(ecs::World& world, data::Handle::SerContext handleSerCtx, data::Handle::DesContext handleDesCtx);
} // namespace cubos::core::ui

#endif // CUBOS_CORE_UI_ECS_HPP
