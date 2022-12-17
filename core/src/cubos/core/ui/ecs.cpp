#include <cubos/core/ui/ecs.hpp>
#include <cubos/core/ui/serialization.hpp>

#include <imgui.h>

void cubos::core::ui::showWorld(ecs::Debug debug)
{
    ImGui::BeginTable("showWorld", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
    for (auto [entity, pkg] : debug)
    {
        ui::showPackage(pkg, std::to_string(entity.index));
    }
    ImGui::EndTable();
}

void cubos::core::ui::editWorld(ecs::Debug debug, data::Handle::DesContext handleCtx)
{
    ImGui::BeginTable("editWorld", 3, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
    for (auto [entity, pkg] : debug)
    {
        if (ui::editPackage(pkg, std::to_string(entity.index)))
        {
            debug.unpack(entity, pkg, handleCtx);
        }
    }
    ImGui::EndTable();
}
