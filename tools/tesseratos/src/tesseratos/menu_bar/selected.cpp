#include "selected.hpp"

#include <cubos/core/ecs/reflection.hpp>

CUBOS_REFLECT_IMPL(tesseratos::MenuBarSelected)
{
    return cubos::core::ecs::TypeBuilder<MenuBarSelected>("tesseratos::MenuBarSelected").build();
}
