#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/render/hdr/hdr.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::HDR)
{
    return core::ecs::TypeBuilder<HDR>("cubos::engine::HDR").withField("size", &HDR::size).build();
}
