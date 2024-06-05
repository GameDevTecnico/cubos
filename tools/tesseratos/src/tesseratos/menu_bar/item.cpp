#include "item.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

CUBOS_REFLECT_IMPL(tesseratos::MenuBarItem)
{
    return cubos::core::ecs::TypeBuilder<MenuBarItem>("tesseratos::MenuBarItem")
        .withField("name", &MenuBarItem::name)
        .withField("order", &MenuBarItem::order)
        .build();
}
