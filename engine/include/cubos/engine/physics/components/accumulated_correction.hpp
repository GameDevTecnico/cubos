/// @file
/// @brief Component @ref cubos::engine::AccumulatedCorrection.
/// @ingroup physics-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds the corrections accumulated from the constraints solving.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API AccumulatedCorrection
    {
        CUBOS_REFLECT;

        glm::vec3 position = {0.0F, 0.0F, 0.0F}; ///< Accumulated position correction.
        float impulse = 0.0F;                    ///< Accumulated impulse.
    };
} // namespace cubos::engine
