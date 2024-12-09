#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/render/picker/picker.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/tools/selection/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::World;

void cubos::engine::selectionPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(renderPickerPlugin);

    cubos.resource<Selection>();

    cubos.system("select entities on mouse click")
        .after(drawToRenderPickerTag)
        .call([](Query<const RenderPicker&, const RenderTarget&> pickers, const World& world, Selection& selection) {
            if (ImGui::GetIO().WantCaptureMouse)
            {
                return;
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                for (auto [picker, target] : pickers)
                {
                    // Find the render picker for the main window (nullptr framebuffer)
                    if (target.framebuffer == nullptr)
                    {
                        uint32_t entityId =
                            picker.read(static_cast<unsigned int>((ImGui::GetMousePos().x / ImGui::GetWindowWidth()) *
                                                                  static_cast<float>(target.size.x)),
                                        static_cast<unsigned int>((ImGui::GetMousePos().y / ImGui::GetWindowHeight()) *
                                                                  static_cast<float>(target.size.y)));

                        if (entityId == UINT32_MAX)
                        {
                            selection.entity = Entity{};
                        }
                        else if (entityId < static_cast<uint32_t>(1 << 31))
                        {
                            selection.entity = Entity{entityId, world.generation(entityId)};
                        }

                        break;
                    }
                }
            }
        });
}
