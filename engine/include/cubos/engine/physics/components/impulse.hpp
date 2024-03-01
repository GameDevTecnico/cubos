/// @file
/// @brief Component @ref cubos::engine::Force.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds impulses applied on a particle.
    /// @note Should be used with @ref Velocity.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Impulse
    {
        CUBOS_REFLECT;

        glm::vec3 vec() const
        {
            return mImpulse;
        }

        void add(glm::vec3 impulse)
        {
            mImpulse += impulse;
        }

        void clear()
        {
            mImpulse = {0.0F, 0.0F, 0.0F};
        }

    private:
        glm::vec3 mImpulse;
    };

} // namespace cubos::engine
