#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/scale.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Scale)
{
    return core::ecs::ComponentTypeBuilder<Scale>("cubos::engine::Scale").withField("mat", &Scale::factor).build();
}
