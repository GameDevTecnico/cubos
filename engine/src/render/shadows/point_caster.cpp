#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadows/point_caster.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::PointShadowCaster)
{
    return core::ecs::TypeBuilder<PointShadowCaster>("cubos::engine::PointShadowCaster")
        .withField("baseSettings", &PointShadowCaster::baseSettings)
        .build();
}
