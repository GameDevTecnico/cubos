/// @dir
/// @brief @ref tesseratos-entity-inspector-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-entity-inspector-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-entity-inspector-plugin Entity inspector
    /// @ingroup tesseratos
    /// @brief Allows inspecting and modifying the components of the selected entity through a
    /// ImGui window.
    ///
    /// @note The selected entity is identified by the @ref EntitySelector resource.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref tesseratos-entity-selector-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-entity-inspector-plugin
    void entityInspectorPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
