#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/camera/perspective.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PerspectiveCamera)
{
    return core::ecs::TypeBuilder<PerspectiveCamera>("cubos::engine::PerspectiveCamera")
        .withField("fovY", &PerspectiveCamera::fovY)
        .build();
}
