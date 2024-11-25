#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadows/casters/spot_caster.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::SpotShadowCaster)
{
    return core::ecs::TypeBuilder<SpotShadowCaster>("cubos::engine::SpotShadowCaster")
        .withField("baseSettings", &SpotShadowCaster::baseSettings)
        .build();
}
