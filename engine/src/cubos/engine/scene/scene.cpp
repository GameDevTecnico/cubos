#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/scene/scene.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Scene)
{
    return core::reflection::Type::create("cubos::engine::Scene");
}
