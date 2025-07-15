/// @file
/// @brief Component @ref cubos::engine::Particle.Spawner.
/// @ingroup render-particles-plugin
#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Represents a particle in the particle system.
    /// @ingroup render-particles-plugin
    struct CUBOS_ENGINE_API Particle
    {

        CUBOS_REFLECT;
        /// @brief The position of the particle.
        glm::vec3 position;

        /// @brief The velocity of the particle.
        glm::vec3 velocity;

        /// @brief The lifetime of the particle.
        float lifetime = 0.0f;

        /// @brief The age of the particle.
        float age = 0.0f;
    };
} // namespace cubos::engine