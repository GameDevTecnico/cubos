/// @file
/// @brief Component @ref cubos::engine::AccumulatedCorrection.
/// @ingroup physics-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which holds the corrections accumulated from the constraints solving.
    /// @ingroup physics-plugin
    struct AccumulatedCorrection
    {
        CUBOS_REFLECT;

        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< Accumulated correction.
    };
} // namespace cubos::engine
