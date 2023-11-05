/// @file
/// @brief Resource @ref cubos::engine::Gravity.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

namespace cubos::engine
{
    /// @brief Resouce which holds the global value of gravity.
    /// @ingroup physics-plugin
    struct Gravity
    {
        glm::vec3 value = {0.0f, -9.81f, 0.0f};
    };
} // namespace cubos::engine
