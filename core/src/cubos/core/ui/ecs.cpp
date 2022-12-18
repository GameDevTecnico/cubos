#include <cubos/core/ui/ecs.hpp>
#include <cubos/core/ui/serialization.hpp>

#include <imgui.h>

void cubos::core::ui::showWorld(const ecs::World& world, data::Handle::SerContext handleCtx)
{
    ImGui::BeginTable("showWorld", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);

    for (auto entity : world)
    {
        auto pkg = world.pack(entity, handleCtx);
        ui::showPackage(pkg, std::to_string(entity.index));
    }
    ImGui::EndTable();
}

void cubos::core::ui::editWorld(ecs::World& world, data::Handle::SerContext handleSerCtx,
                                data::Handle::DesContext handleDesCtx)
{
    ImGui::BeginTable("editWorld", 3, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
    for (auto entity : world)
    {
        auto pkg = world.pack(entity, handleSerCtx);
        if (ui::editPackage(pkg, std::to_string(entity.index)))
        {
            world.unpack(entity, pkg, handleDesCtx);
        }
    }
    ImGui::EndTable();
}
