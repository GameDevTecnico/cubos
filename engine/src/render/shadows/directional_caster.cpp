#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/render/shadows/directional_caster.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::DirectionalShadowCaster)
{
    return core::ecs::TypeBuilder<DirectionalShadowCaster>("cubos::engine::DirectionalShadowCaster")
        .withField("baseSettings", &DirectionalShadowCaster::baseSettings)
        .withField("numSplits", &DirectionalShadowCaster::numSplits)
        .withField("splitDistances", &DirectionalShadowCaster::splitDistances)
        .withField("maxDistance", &DirectionalShadowCaster::maxDistance)
        .build();
}
