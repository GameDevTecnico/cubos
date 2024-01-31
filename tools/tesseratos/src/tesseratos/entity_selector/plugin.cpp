#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/screen_picker/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include <tesseratos/entity_selector/plugin.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::World;

using cubos::core::io::MouseButton;
using cubos::core::io::MouseButtonEvent;
using cubos::core::io::MouseMoveEvent;
using cubos::core::io::WindowEvent;

using cubos::engine::Cubos;
using cubos::engine::EventReader;
using cubos::engine::ScreenPicker;

using namespace tesseratos;

void tesseratos::entitySelectorPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(cubos::engine::screenPickerPlugin);
    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.addResource<EntitySelector>();

    cubos.startupSystem("initialize EntitySelector")
        .tagged("cubos.entitySelector.init")
        .call([](EntitySelector& entitySelector) {
            entitySelector.selection = Entity{};
            entitySelector.lastMousePosition = glm::ivec2{0, 0};
        });

    cubos.system("process window input for EntitySelector")
        .tagged("cubos.entitySelector.input")
        .after("cubos.window.poll")
        .after("cubos.renderer.draw")
        .call([](const ScreenPicker& screenPicker, EntitySelector& entitySelector, const World& world,
                 EventReader<WindowEvent> windowEvent) {
            for (const auto& event : windowEvent)
            {
                if (ImGui::GetIO().WantCaptureMouse)
                {
                    // Consume event but don't do anything
                    continue;
                }

                if (std::holds_alternative<MouseMoveEvent>(event))
                {
                    entitySelector.lastMousePosition = std::get<MouseMoveEvent>(event).position;
                }
                else if (std::holds_alternative<MouseButtonEvent>(event))
                {
                    if (std::get<MouseButtonEvent>(event).button == cubos::core::io::MouseButton::Left)
                    {
                        if (!std::get<MouseButtonEvent>(event).pressed)
                        {
                            uint32_t entityId =
                                screenPicker.at(entitySelector.lastMousePosition.x, entitySelector.lastMousePosition.y);

                            if (entityId == UINT32_MAX)
                            {
                                entitySelector.selection = Entity{};
                            }
                            else
                            {
                                entitySelector.selection = Entity{entityId, world.generation(entityId)};
                            }
                        }
                    }
                }
            }
        });
}
