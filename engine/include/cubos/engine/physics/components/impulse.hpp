/// @file
/// @brief Component @ref cubos::engine::Impulse.
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

        // Angular impulse resulting from addImpulseOnPoint.
        glm::vec3 angularImpulse() const
        {
            return mAngularImpulse;
        }

        void add(glm::vec3 impulse)
        {
            mImpulse += impulse;
        }

        void addImpulseOnPoint(glm::vec3 impulse, glm::vec3 localPoint, glm::vec3 centerOfMass)
        {
            mImpulse += impulse;
            mAngularImpulse += glm::cross(localPoint - centerOfMass, impulse);
        }

        void clear()
        {
            mImpulse = {0.0F, 0.0F, 0.0F};
            mAngularImpulse = {0.0F, 0.0F, 0.0F};
        }

    private:
        glm::vec3 mImpulse;
        glm::vec3 mAngularImpulse;
    };

} // namespace cubos::engine
