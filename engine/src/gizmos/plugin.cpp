#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/depth/depth.hpp>
#include <cubos/engine/render/depth/plugin.hpp>
#include <cubos/engine/render/picker/picker.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "renderer.hpp"

CUBOS_DEFINE_TAG(cubos::engine::gizmosInitTag);
CUBOS_DEFINE_TAG(cubos::engine::gizmosInputTag);
CUBOS_DEFINE_TAG(cubos::engine::gizmosDrawTag);

using cubos::core::gl::Framebuffer;
using cubos::core::gl::FramebufferDesc;
using cubos::core::gl::Texture2D;

using cubos::core::io::MouseButtonEvent;
using cubos::core::io::MouseMoveEvent;
using cubos::core::io::ResizeEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

using namespace cubos::engine;

static void decreaseGizmoLifespans(std::vector<std::shared_ptr<Gizmos::Gizmo>>& gizmos, const DeltaTime& dt)
{
    for (std::size_t i = 0; i < gizmos.size(); i++)
    {
        if (gizmos[i]->decreaseLifespan(dt.unscaledValue))
        {
            gizmos[i] = gizmos.back();
            gizmos.pop_back();
            i--;
        }
    }
}

void cubos::engine::gizmosPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(renderPickerPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(renderDepthPlugin);

    cubos.resource<Gizmos>();
    cubos.resource<GizmosRenderer>();

    cubos.component<GizmosTarget>();

    cubos.startupTag(gizmosInitTag);

    cubos.tag(gizmosInputTag);
    cubos.tag(gizmosDrawTag);

    cubos.startupSystem("initialize GizmosRenderer")
        .tagged(gizmosInitTag)
        .after(windowInitTag)
        .call(
            [](GizmosRenderer& gizmosRenderer, const Window& window) { gizmosRenderer.init(&window->renderDevice()); });

    cubos.system("process Gizmos input")
        .tagged(gizmosInputTag)
        .after(windowPollTag)
        .before(gizmosDrawTag)
        .call([](GizmosRenderer& gizmosRenderer, Gizmos& gizmos, EventReader<WindowEvent> windowEvent) {
            gizmos.mLocking = false;
            for (const auto& event : windowEvent)
            {
                if (std::holds_alternative<MouseMoveEvent>(event))
                {
                    gizmosRenderer.lastMousePosition = std::get<MouseMoveEvent>(event).position;
                }
                else if (std::holds_alternative<MouseButtonEvent>(event))
                {
                    if (std::get<MouseButtonEvent>(event).button == cubos::core::io::MouseButton::Left)
                    {
                        gizmosRenderer.mousePressed = std::get<MouseButtonEvent>(event).pressed;
                        if (!std::get<MouseButtonEvent>(event).pressed)
                        {
                            gizmos.releaseLocked();
                        }
                        else
                        {
                            gizmos.mLocking = true;
                        }
                    }
                }
            }
        });

    cubos.system("draw Gizmos")
        .tagged(gizmosDrawTag)
        .tagged(drawToRenderPickerTag)
        .call([](Gizmos& gizmos, GizmosRenderer& gizmosRenderer, const DeltaTime& deltaTime,
                 Query<Entity, RenderTarget&, Opt<RenderPicker&>, RenderDepth&, GizmosTarget&> targets,
                 Query<Entity, const LocalToWorld&, const Camera&, const DrawsTo&> cameras) {
            auto& rd = *gizmosRenderer.renderDevice;
            auto orthoVP =
                glm::translate(glm::mat4(1.0F), glm::vec3(-1.0F, -1.0F, 0.0F)) * glm::ortho(-0.5F, 0.5F, -0.5F, 0.5F);

            // Draw gizmos to each of the requested targets.
            for (auto [targetEnt, target, picker, depth, gizmosTarget] : targets)
            {
                // Prepare a framebuffer for drawing to the picker texture, if necessary.
                if (picker.contains() && picker.value().frontTexture != nullptr)
                {
                    if (gizmosTarget.frontPicker != picker.value().frontTexture)
                    {
                        std::swap(gizmosTarget.frontPicker, gizmosTarget.backPicker);
                        std::swap(gizmosTarget.frontDepth, gizmosTarget.backDepth);
                        std::swap(gizmosTarget.frontFramebuffer, gizmosTarget.backFramebuffer);
                    }
                    if (gizmosTarget.frontPicker != picker.value().frontTexture ||
                        gizmosTarget.frontDepth != depth.texture)
                    {
                        FramebufferDesc desc{};
                        desc.targetCount = 1;
                        desc.targets[0].setTexture2DTarget(picker.value().frontTexture);
                        desc.depthStencil.setTexture2DTarget(depth.texture);
                        gizmosTarget.frontPicker = picker.value().frontTexture;
                        gizmosTarget.frontDepth = depth.texture;
                        gizmosTarget.frontFramebuffer = rd.createFramebuffer(desc);
                    }
                }

                // Prepare for color pass.
                rd.setRasterState(nullptr);
                rd.setBlendState(nullptr);
                rd.setFramebuffer(target.framebuffer);
                if (!target.cleared)
                {
                    rd.clearColor(0.0F, 0.0F, 0.0F, 0.0F);
                    rd.clearDepth(1.0F);
                    target.cleared = true;
                }
                rd.setShaderPipeline(gizmosRenderer.drawPipeline);

                // Draw world and view-space gizmos.
                for (auto [cameraEntity, localToWorld, camera, drawsTo] : cameras.pin(1, targetEnt))
                {
                    if (!camera.active)
                    {
                        continue;
                    }

                    // Prepare the viewport and scissor rectangles.
                    rd.setViewport(static_cast<int>(drawsTo.viewportOffset.x * static_cast<float>(target.size.x)),
                                   static_cast<int>(drawsTo.viewportOffset.y * static_cast<float>(target.size.y)),
                                   static_cast<int>(drawsTo.viewportSize.x * static_cast<float>(target.size.x)),
                                   static_cast<int>(drawsTo.viewportSize.y * static_cast<float>(target.size.y)));
                    rd.setScissor(static_cast<int>(drawsTo.viewportOffset.x * static_cast<float>(target.size.x)),
                                  static_cast<int>(drawsTo.viewportOffset.y * static_cast<float>(target.size.y)),
                                  static_cast<int>(drawsTo.viewportSize.x * static_cast<float>(target.size.x)),
                                  static_cast<int>(drawsTo.viewportSize.y * static_cast<float>(target.size.y)));

                    // Prepare camera view projection matrix.
                    auto worldVP = camera.projection * glm::inverse(localToWorld.mat);

                    // Draw world-space gizmos.
                    rd.setDepthStencilState(gizmosRenderer.doDepthCheckStencilState);
                    for (auto& gizmo : gizmos.worldGizmos)
                    {
                        if (gizmo->drawCameras.empty() ||
                            std::find(gizmo->drawCameras.begin(), gizmo->drawCameras.end(), cameraEntity) !=
                                gizmo->drawCameras.end())
                        {
                            gizmo->draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Color, worldVP);
                        }
                    }

                    // Draw view-space gizmos.
                    rd.setDepthStencilState(gizmosRenderer.noDepthCheckStencilState);
                    for (auto& gizmo : gizmos.viewGizmos)
                    {
                        if (gizmo->drawCameras.empty() ||
                            std::find(gizmo->drawCameras.begin(), gizmo->drawCameras.end(), cameraEntity) !=
                                gizmo->drawCameras.end())
                        {
                            gizmo->draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Color, orthoVP);
                        }
                    }
                }

                // Draw screen-space gizmos.
                rd.setViewport(0, 0, static_cast<int>(target.size.x), static_cast<int>(target.size.y));
                rd.setScissor(0, 0, static_cast<int>(target.size.x), static_cast<int>(target.size.y));
                for (auto& gizmo : gizmos.screenGizmos)
                {
                    gizmo->draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Color, orthoVP);
                }

                // Prepare for picking pass.
                if (!picker.contains() || picker.value().frontTexture == nullptr)
                {
                    continue;
                }

                rd.setFramebuffer(gizmosTarget.frontFramebuffer);
                if (!picker.value().cleared)
                {
                    rd.clearTargetColor(0, UINT16_MAX, UINT16_MAX, 0, 0);
                    rd.clearDepth(1.0F);
                    picker.value().cleared = true;
                }
                rd.setShaderPipeline(gizmosRenderer.idPipeline);
                auto* idBP = gizmosRenderer.idPipeline->getBindingPoint("gizmo");

                // Draw world and view-space gizmos.
                for (auto [cameraEntity, localToWorld, camera, drawsTo] : cameras.pin(1, targetEnt))
                {
                    if (!camera.active)
                    {
                        continue;
                    }

                    // Prepare the viewport and scissor rectangles.
                    rd.setViewport(static_cast<int>(drawsTo.viewportOffset.x * static_cast<float>(target.size.x)),
                                   static_cast<int>(drawsTo.viewportOffset.y * static_cast<float>(target.size.y)),
                                   static_cast<int>(drawsTo.viewportSize.x * static_cast<float>(target.size.x)),
                                   static_cast<int>(drawsTo.viewportSize.y * static_cast<float>(target.size.y)));
                    rd.setScissor(static_cast<int>(drawsTo.viewportOffset.x * static_cast<float>(target.size.x)),
                                  static_cast<int>(drawsTo.viewportOffset.y * static_cast<float>(target.size.y)),
                                  static_cast<int>(drawsTo.viewportSize.x * static_cast<float>(target.size.x)),
                                  static_cast<int>(drawsTo.viewportSize.y * static_cast<float>(target.size.y)));

                    // Prepare camera view projection matrix.
                    auto worldVP = camera.projection * glm::inverse(localToWorld.mat);

                    // Draw world-space gizmos.
                    rd.setDepthStencilState(gizmosRenderer.doDepthCheckStencilState);
                    for (auto& gizmo : gizmos.worldGizmos)
                    {
                        if (gizmo->drawCameras.empty() ||
                            std::find(gizmo->drawCameras.begin(), gizmo->drawCameras.end(), cameraEntity) !=
                                gizmo->drawCameras.end())
                        {
                            idBP->setConstant(gizmo->id);
                            gizmo->draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Id, worldVP);
                        }
                    }

                    // Draw view-space gizmos.
                    rd.setDepthStencilState(gizmosRenderer.noDepthCheckStencilState);
                    for (auto& gizmo : gizmos.viewGizmos)
                    {
                        if (gizmo->drawCameras.empty() ||
                            std::find(gizmo->drawCameras.begin(), gizmo->drawCameras.end(), cameraEntity) !=
                                gizmo->drawCameras.end())
                        {
                            idBP->setConstant(gizmo->id);
                            gizmo->draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Id, orthoVP);
                        }
                    }
                }

                // Draw screen-space gizmos.
                rd.setViewport(0, 0, static_cast<int>(target.size.x), static_cast<int>(target.size.y));
                rd.setScissor(0, 0, static_cast<int>(target.size.x), static_cast<int>(target.size.y));
                for (auto& gizmo : gizmos.screenGizmos)
                {
                    idBP->setConstant(gizmo->id);
                    gizmo->draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Id, orthoVP);
                }
            }

            // Decrease lifespan of gizmos and remove dead ones.
            decreaseGizmoLifespans(gizmos.worldGizmos, deltaTime);
            decreaseGizmoLifespans(gizmos.viewGizmos, deltaTime);
            decreaseGizmoLifespans(gizmos.screenGizmos, deltaTime);
        });

    cubos.system("do Gizmos picking")
        .after(drawToRenderPickerTag)
        .call([](const Window& window, GizmosRenderer& gizmosRenderer, Gizmos& gizmos,
                 Query<const RenderTarget&, const RenderPicker&> renderPickers) {
            int mouseX = gizmosRenderer.lastMousePosition.x;
            int mouseY = gizmosRenderer.lastMousePosition.y;

            // Find the render picker of the screen.
            for (auto [target, picker] : renderPickers)
            {
                if (target.framebuffer == nullptr)
                {
                    mouseX = static_cast<int>(static_cast<float>(mouseX) * static_cast<float>(target.size.x) /
                                              static_cast<float>(window->size().x));
                    mouseY = static_cast<int>(static_cast<float>(mouseY) * static_cast<float>(target.size.y) /
                                              static_cast<float>(window->size().y));

                    auto id = picker.read(static_cast<unsigned int>(mouseX), static_cast<unsigned int>(mouseY));
                    if (id < static_cast<uint32_t>(1 << 31))
                    {
                        // Not a gizmo, treat it like empty space
                        id = UINT32_MAX;
                    }

                    gizmos.handleInput(id, gizmosRenderer.mousePressed);

                    if (gizmos.mLocking)
                    {
                        gizmos.setLocked(id);
                    }

                    break;
                }
            }
        });
}
