#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/tone_mapping/fxaa.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::FXAA)
{
    return core::ecs::TypeBuilder<FXAA>("cubos::engine::FXAA")
        .withField("edgeThresholdMin", &FXAA::edgeThresholdMin)
        .withField("edgeThresholdMax", &FXAA::edgeThresholdMax)
        .withField("subpixelQuality", &FXAA::subpixelQuality)
        .withField("iterations", &FXAA::iterations)
        .build();
}
