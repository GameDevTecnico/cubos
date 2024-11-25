#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadows/casters/caster.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ShadowCaster)
{
    return core::ecs::TypeBuilder<ShadowCaster>("cubos::engine::ShadowCaster")
        .withField("bias", &ShadowCaster::bias)
        .withField("blurRadius", &ShadowCaster::blurRadius)
        .withField("normalOffsetScale", &ShadowCaster::normalOffsetScale)
        .build();
}
