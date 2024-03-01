/// @file
/// @brief Component @ref cubos::engine::BoxCollisionShape.
/// @ingroup collisions-plugin

#pragma once

#include <cubos/core/geom/box.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which adds a box collision shape to an entity, used with a @ref Collider component.
    /// @ingroup collisions-plugin
    struct CUBOS_ENGINE_API BoxCollisionShape
    {
        CUBOS_REFLECT;

        cubos::core::geom::Box box; ///< Box shape.
    };
} // namespace cubos::engine
