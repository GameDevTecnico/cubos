/// @file
/// @brief Component @ref cubos::engine::FreeCameraController.
/// @ingroup free-camera-controller-plugin

#pragma once

#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which moves the camera.
    /// @ingroup free-camera-controller-plugin
    struct CUBOS_ENGINE_API FreeCameraController
    {
        CUBOS_REFLECT;

        /// @brief Whether the controller is enabled. Mouse is only captured when enabled.
        bool enabled = true;

        /// @brief Whether the controller uses unscaled delta time.
        bool unscaledDeltaTime = false;

        /// @brief Whether to ignore the mouseDelta movement (when the controller switches from disabled to enabled).
        bool ignorePreviousMovement = false;

        /// @brief Action used to move the camera laterally (positive is right).
        std::string lateral = "move-lateral";

        /// @brief Action used to move the camera vertically (positive is up).
        std::string vertical = "move-vertical";

        /// @brief Action used to move the camera longitudinally (positive is forward).
        std::string longitudinal = "move-longitudinal";

        /// @brief Speed at which the camera moves.
        float speed = 6.0F;

        /// @brief Sensitivity of the camera rotation, i.e,, how fast the camera rotates.
        float sens = 5.0F;

        /// @brief Inclination of the camera.
        float phi = 0.0F;

        /// @brief Azimuth of the camera.
        float theta = 0.0F;
    };
} // namespace cubos::engine
