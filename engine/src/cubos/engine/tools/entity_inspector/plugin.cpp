#include <cubos/engine/tools/entity_inspector/plugin.hpp>
#include <imgui.h>

#include <cubos/core/ui/serialization.hpp>

#include <cubos/engine/plugins/imgui.hpp>

#include <cubos/engine/plugins/tools/entity_selector.hpp>

using cubos::core::ecs::World;
using cubos::engine::plugins::tools::EntitySelector;

static void inspectEntity(EntitySelector& selector, World& world)
{
    ImGui::Begin("Entity Inspector");
    if (!ImGui::IsWindowCollapsed())
    {
        ImGui::BeginTable("showEntity", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);

        if (!selector.selection.isNull())
        {
            std::vector<std::string> entityNamesToRemove;
            auto pkg = world.pack(selector.selection,
                                  [](cubos::core::data::Serializer& ser, const cubos::core::data::Handle& handle,
                                     const char* name) { ser.write(handle.getId(), name); });
            cubos::core::ui::showPackage(pkg, std::to_string(selector.selection.index));
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void cubos::engine::tools::entityInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::plugins::imguiPlugin);
    cubos.addPlugin(cubos::engine::plugins::tools::entitySelectorPlugin);

    cubos.system(inspectEntity).tagged("cubos.imgui");
}
