#ifndef CUBOS_ENGINE_ECS_CAMERA_HPP
#define CUBOS_ENGINE_ECS_CAMERA_HPP

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

#endif // CUBOS_ENGINE_ECS_CAMERA_HPP
