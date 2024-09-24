/// @file
/// @brief Resource @ref cubos::engine::Selection.
/// @ingroup selection-tool-plugin

#pragma once

#include <cubos/core/ecs/entity/entity.hpp>

namespace cubos::engine
{
    /// @brief Resource which identifies the current selection.
    /// @ingroup selection-tool-plugin
    struct Selection
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Selected entity, or null if none.
    };
} // namespace cubos::engine
