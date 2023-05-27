#include <imgui.h>

#include <cubos/core/ui/ecs.hpp>
#include <cubos/core/ui/serialization.hpp>

void cubos::core::ui::showWorld(const ecs::World& world, data::Context* context)
{
    ImGui::BeginTable("showWorld", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);

    for (auto entity : world)
    {
        auto pkg = world.pack(entity, context);
        ui::showPackage(pkg, std::to_string(entity.index));
    }
    ImGui::EndTable();
}

void cubos::core::ui::editWorld(ecs::World& world, data::Context* serContext, data::Context* desContext)
{
    ImGui::BeginTable("editWorld", 3, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
    for (auto entity : world)
    {
        auto pkg = world.pack(entity, serContext);
        if (ui::editPackage(pkg, std::to_string(entity.index)))
        {
            world.unpack(entity, pkg, desContext);
        }
    }
    ImGui::EndTable();
}
