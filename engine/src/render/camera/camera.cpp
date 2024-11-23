#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/camera/camera.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Camera)
{
    return core::ecs::TypeBuilder<Camera>("cubos::engine::Camera")
        .withField("projection", &Camera::projection)
        .withField("zNear", &Camera::zNear)
        .withField("zFar", &Camera::zFar)
        .build();
}
