/// @file
/// @brief Component @ref cubos::engine::PreviousPosition.
/// @ingroup physics-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which holds the previous position of the entity.
    /// Used for the integrator on the update velocity step.
    /// @ingroup physics-plugin
    struct [[cubos::component("cubos/previous_position", VecStorage)]] PreviousPosition
    {
        CUBOS_REFLECT;

        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< Previous position of the entity.
    };
} // namespace cubos::engine
