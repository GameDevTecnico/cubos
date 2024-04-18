#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/tone_mapping/tone_mapping.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ToneMapping)
{
    return core::ecs::TypeBuilder<ToneMapping>("cubos::engine::ToneMapping")
        .withField("gamma", &ToneMapping::gamma)
        .withField("exposure", &ToneMapping::exposure)
        .build();
}
