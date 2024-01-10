/// @file
/// @brief Component @ref cubos::engine::Force.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which holds impulses applied on a particle.
    /// @note Should be used with @ref Velocity.
    /// @ingroup physics-plugin
    struct Impulse
    {
        CUBOS_REFLECT;

        glm::vec3 getImpulse() const
        {
            return mImpulse;
        }

        void addImpulse(glm::vec3 impulse)
        {
            mImpulse += impulse;
        }

        void clearImpulse()
        {
            mImpulse = {0.0F, 0.0F, 0.0F};
        }

    private:
        glm::vec3 mImpulse;
    };

} // namespace cubos::engine
