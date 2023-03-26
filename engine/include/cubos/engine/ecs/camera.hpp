#pragma once

namespace cubos::engine::ecs
{
    /// Defines parameters of a camera.
    struct [[cubos::component("cubos/camera", VecStorage)]] Camera
    {
        float fovY;  ///< The vertical field of view in degrees.
        float zNear; ///< The near clipping plane.
        float zFar;  ///< The far clipping plane.
    };
} // namespace cubos::engine::ecs
