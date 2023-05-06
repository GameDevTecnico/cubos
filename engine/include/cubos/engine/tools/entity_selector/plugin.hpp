#pragma once

#include <cubos/core/ecs/entity_manager.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// Resource used to identify the currently selected entity. Used by multiple editor plugins.
    struct EntitySelector
    {
        cubos::core::ecs::Entity selection;
    };

    /// Plugin that provides a resource used to identify the currently selected entity in the
    /// editor or other tools.
    ///
    /// @details This plugin only provides the resource. It is used just to avoid duplicating
    /// the resource definition in multiple plugins.
    ///
    /// Resources:
    /// - `EntitySelector`: the currently selected entity.
    ///
    /// @param cubos CUBOS. main class
    void entitySelectorPlugin(Cubos& cubos);
}; // namespace cubos::engine::tools
