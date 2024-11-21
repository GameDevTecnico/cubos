#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/imgui/context.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ImGuiContextHolder)
{
    return core::ecs::TypeBuilder<ImGuiContextHolder>("cubos::engine::ImGuiContextHolder").build();
}
