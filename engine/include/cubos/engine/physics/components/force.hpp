/// @file
/// @brief Component @ref cubos::engine::Force.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which holds forces applied on a particle.
    /// @note Should be used with @ref Velocity.
    /// @ingroup physics-plugin
    struct Force
    {
        CUBOS_REFLECT;

        glm::vec3 vec() const
        {
            return mForce;
        }

        void add(glm::vec3 force)
        {
            mForce += force;
        }

        void clear()
        {
            mForce = {0.0F, 0.0F, 0.0F};
        }

    private:
        glm::vec3 mForce = {0.0F, 0.0F, 0.0F};
    };

} // namespace cubos::engine
