/// @file
/// @brief Relation @ref cubos::engine::IntersectionStart.
/// @ingroup collisions-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Relation which identifies the frame when two entities start colliding.
    ///
    /// This relation becomes obsolete when observers support relations.
    /// @ingroup collisions-plugin
    struct CUBOS_ENGINE_API IntersectionStart
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
