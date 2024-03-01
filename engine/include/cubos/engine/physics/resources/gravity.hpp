/// @file
/// @brief Resource @ref cubos::engine::Gravity.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds the global value of gravity.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Gravity
    {
        CUBOS_REFLECT;

        glm::vec3 value = {0.0F, -9.81F, 0.0F};
    };
} // namespace cubos::engine
