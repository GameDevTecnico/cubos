#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/carray.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/render/lights/environment.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderEnvironment)
{
    return core::ecs::TypeBuilder<RenderEnvironment>("cubos::engine::RenderEnvironment")
        .withField("ambient", &RenderEnvironment::ambient)
        .withField("skyGradient", &RenderEnvironment::skyGradient)
        .build();
}
