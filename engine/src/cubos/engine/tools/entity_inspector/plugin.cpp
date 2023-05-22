#include <imgui.h>

#include <cubos/core/ui/serialization.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/entity_inspector/plugin.hpp>
#include <cubos/engine/tools/entity_selector/plugin.hpp>

using cubos::core::data::Context;
using cubos::core::data::SerializationMap;
using cubos::core::ecs::Entity;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;
using cubos::core::ui::showPackage;
using namespace cubos::engine;

static void inspectEntity(Write<World> world)
{
    auto selection = world->read<tools::EntitySelector>().get().selection;

    ImGui::Begin("Entity Inspector");
    if (!ImGui::IsWindowCollapsed())
    {
        ImGui::BeginTable("showEntity", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);

        if (!selection.isNull())
        {
            // Setup a context with a serialization map which simply converts the entity indices to a string.
            // TODO: maybe there should be a global serialization map for this, since its such a common behaviour?
            Context context{};
            context.push(SerializationMap<Entity, std::string>{[](const Entity& entity, std::string& string) {
                                                                   string = std::to_string(entity.index);
                                                                   return true;
                                                               },
                                                               [](Entity&, const std::string&) { return false; }});

            auto pkg = world->pack(selection, context);
            showPackage(pkg, std::to_string(selection.index));
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void cubos::engine::tools::entityInspectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(imguiPlugin);
    cubos.addPlugin(tools::entitySelectorPlugin);

    cubos.system(inspectEntity).tagged("cubos.imgui");
}
