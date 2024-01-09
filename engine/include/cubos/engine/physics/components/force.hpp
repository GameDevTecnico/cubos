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
    public:
        CUBOS_REFLECT;

        const glm::vec3 getForce() const
        {
            return mForce;
        }

        void addForce(glm::vec3 force)
        {
            mForce += force;
        }

        void clearForce()
        {
            mForce = {0.0f, 0.0f, 0.0f};
        }

    private:
        glm::vec3 mForce = {0.0F, 0.0F, 0.0F};
    };

} // namespace cubos::engine
