/// @dir
/// @brief @ref entity-selector-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup entity-selector-tool-plugin

#pragma once

#include <cubos/core/ecs/entity/manager.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::tools
{
    /// @defgroup entity-selector-tool-plugin Entity selector
    /// @ingroup tool-plugins
    /// @brief Adds a resource used to select an entity.
    ///
    /// This plugins exists to reduce coupling between plugins. For example, a plugin which allows
    /// selecting entities through a ImGui window only needs to depend on this plugin, instead of
    /// having to know about all the plugins which care about it. The same applies in the other
    /// direction.
    ///
    /// ## Resources
    /// - @ref EntitySelector - identifies the currently selected entity.

    /// @brief Resource which identifies the currently selected entity.
    struct EntitySelector
    {
        cubos::core::ecs::Entity selection; ///< Selected entity, or `null` if none.
    };

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup entity-selector-tool-plugin
    void entitySelectorPlugin(Cubos& cubos);
} // namespace cubos::engine::tools
