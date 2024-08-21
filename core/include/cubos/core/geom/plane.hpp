/// @file
/// @brief Class @ref cubos::core::geom::Plane.
/// @ingroup core-geom

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a plane. Assumes equation: normal.x + normal.y + normal.z + d = 0
    /// @ingroup core-geom
    struct CUBOS_CORE_API Plane
    {
        CUBOS_REFLECT;

        glm::vec3 normal;
        float d;
    };
} // namespace cubos::core::geom
