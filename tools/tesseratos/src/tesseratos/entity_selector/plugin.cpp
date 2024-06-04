#include "plugin.hpp"

#include <imgui.h>

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/render/picker/picker.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::Entity;
using cubos::core::ecs::World;

using cubos::core::io::MouseButton;
using cubos::core::io::MouseButtonEvent;
using cubos::core::io::MouseMoveEvent;
using cubos::core::io::WindowEvent;

using cubos::engine::Cubos;
using cubos::engine::EventReader;
using cubos::engine::Query;
using cubos::engine::RenderPicker;
using cubos::engine::RenderTarget;

using namespace tesseratos;

CUBOS_REFLECT_IMPL(EntitySelector)
{
    return cubos::core::ecs::TypeBuilder<EntitySelector>("tesseratos::EntitySelector")
        .withField("selection", &EntitySelector::selection)
        .build();
}

void tesseratos::entitySelectorPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(cubos::engine::windowPlugin);
    cubos.depends(cubos::engine::renderTargetPlugin);
    cubos.depends(cubos::engine::renderPickerPlugin);

    cubos.resource<EntitySelector>();

    cubos.startupSystem("initialize EntitySelector").call([](EntitySelector& entitySelector) {
        entitySelector.selection = Entity{};
        entitySelector.lastMousePosition = glm::ivec2{0, 0};
    });

    cubos.system("process window input for EntitySelector")
        .after(cubos::engine::windowPollTag)
        .after(cubos::engine::drawToRenderPickerTag)
        .call([](Query<const RenderPicker&, const RenderTarget&> pickers, EntitySelector& entitySelector,
                 const World& world, EventReader<WindowEvent> windowEvent) {
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
                        if (std::get<MouseButtonEvent>(event).pressed)
                        {
                            for (auto [picker, target] : pickers)
                            {
                                if (target.framebuffer == nullptr)
                                {
                                    uint32_t entityId =
                                        picker.read(static_cast<unsigned int>(entitySelector.lastMousePosition.x),
                                                    static_cast<unsigned int>(entitySelector.lastMousePosition.y));

                                    if (entityId == UINT32_MAX)
                                    {
                                        entitySelector.selection = Entity{};
                                    }
                                    else if (entityId < static_cast<uint32_t>(1 << 31))
                                    {
                                        entitySelector.selection = Entity{entityId, world.generation(entityId)};
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        });
}
