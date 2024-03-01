/// @file
/// @brief Component @ref cubos::engine::PreviousPosition.
/// @ingroup physics-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds the previous position of the entity.
    /// Used for the integrator on the update velocity step.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API PreviousPosition
    {
        CUBOS_REFLECT;

        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< Previous position of the entity.
    };
} // namespace cubos::engine
