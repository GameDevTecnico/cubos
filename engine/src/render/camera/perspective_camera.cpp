#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/camera/perspective_camera.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PerspectiveCamera)
{
    return core::ecs::TypeBuilder<PerspectiveCamera>("cubos::engine::PerspectiveCamera")
        .withField("active", &PerspectiveCamera::active)
        .withField("fovY", &PerspectiveCamera::fovY)
        .withField("zNear", &PerspectiveCamera::zNear)
        .withField("zFar", &PerspectiveCamera::zFar)
        .build();
}
