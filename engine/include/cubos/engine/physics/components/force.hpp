/// @file
/// @brief Component @ref cubos::engine::Force.
/// @ingroup physics-plugin

#pragma once

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds forces applied on a particle.
    /// @note Should be used with @ref Velocity.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Force
    {
        CUBOS_REFLECT;

        glm::vec3 vec() const
        {
            return mForce;
        }

        // Torque resulting from addForceOnPoint.
        glm::vec3 torque() const
        {
            return mTorque;
        }

        void add(glm::vec3 force)
        {
            mForce += force;
        }

        void addForceOnPoint(glm::vec3 force, glm::vec3 localPoint, glm::vec3 centerOfMass)
        {
            mForce += force;
            mTorque += glm::cross(localPoint - centerOfMass, force);
        }

        void clear()
        {
            mForce = {0.0F, 0.0F, 0.0F};
            mTorque = {0.0F, 0.0F, 0.0F};
        }

    private:
        glm::vec3 mForce = {0.0F, 0.0F, 0.0F};
        glm::vec3 mTorque = {0.0F, 0.0F, 0.0F};
    };

} // namespace cubos::engine
