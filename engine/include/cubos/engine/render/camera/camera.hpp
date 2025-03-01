/// @file
/// @brief Component @ref cubos::engine::Camera
/// @ingroup render-camera-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/core/geom/frustum.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Generic component to hold the projection matrix of a specific camera (either perspective or orthogonal).
    /// @note Added automatically once a specific camera is added (e.g @ref cubos::engine::PerspectiveCamera).
    /// @ingroup render-camera-plugin
    struct CUBOS_ENGINE_API Camera
    {
        CUBOS_REFLECT;

        /// @brief Whether the camera is drawing to a target.
        bool active{true};

        /// @brief Projection matrix of the camera.
        glm::mat4 projection{};

        /// @brief Near clipping plane.
        float zNear = 0.1F;

        /// @brief Far clipping plane.
        float zFar = 1000.0F;

        /// @brief Frustum of the projection.
        core::geom::Frustum frustum{};

        /// @brief If the frustum should stop being updated.
        bool freezeFrustum{false};
    };
} // namespace cubos::engine
