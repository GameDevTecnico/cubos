#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/renderer/camera.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Camera)
{
    return core::ecs::TypeBuilder<Camera>("cubos::engine::Camera")
        .withField("fovY", &Camera::fovY)
        .withField("zNear", &Camera::zNear)
        .withField("zFar", &Camera::zFar)
        .build();
}
