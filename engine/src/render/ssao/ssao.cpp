#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/ssao/ssao.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::SSAO)
{
    return core::ecs::TypeBuilder<SSAO>("cubos::engine::SSAO")
        .withField("size", &SSAO::size)
        .withField("resolutionScale", &SSAO::resolutionScale)
        .withField("samples", &SSAO::samples)
        .withField("radius", &SSAO::radius)
        .withField("bias", &SSAO::bias)
        .build();
}
