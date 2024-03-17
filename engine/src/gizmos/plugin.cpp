#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/screen_picker/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "renderer.hpp"

CUBOS_DEFINE_TAG(cubos::engine::GizmosInitTag);
CUBOS_DEFINE_TAG(cubos::engine::GizmosInputTag);
CUBOS_DEFINE_TAG(cubos::engine::GizmosDrawTag);
CUBOS_DEFINE_TAG(cubos::engine::GizmosPickTag);

using cubos::core::gl::Texture2D;

using cubos::core::io::MouseButtonEvent;
using cubos::core::io::MouseMoveEvent;
using cubos::core::io::ResizeEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

using namespace cubos::engine;

static void iterateGizmos(std::vector<std::shared_ptr<Gizmos::Gizmo>>& gizmosVector,
                          const std::vector<std::pair<glm::mat4, BaseRenderer::Viewport>>& cameras,
                          GizmosRenderer& gizmosRenderer, ScreenPicker& screenPicker, const DeltaTime& deltaTime)
{
    std::vector<std::size_t> toRemove;

    for (std::size_t i = 0; i < gizmosVector.size(); i++)
    {
        auto& gizmo = *(gizmosVector[i]);
        for (const auto& [mvp, viewport] : cameras)
        {
            gizmosRenderer.renderDevice->setViewport(viewport.position.x, viewport.position.y, viewport.size.x,
                                                     viewport.size.y);

            // Color
            gizmosRenderer.renderDevice->setShaderPipeline(gizmosRenderer.drawPipeline);
            gizmosRenderer.renderDevice->setFramebuffer(nullptr);
            gizmo.draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Color, mvp);

            // Id
            gizmosRenderer.renderDevice->setShaderPipeline(gizmosRenderer.idPipeline);
            gizmosRenderer.renderDevice->setFramebuffer(screenPicker.framebuffer());

            gizmosRenderer.idPipeline->getBindingPoint("gizmo")->setConstant(gizmo.id);

            gizmo.draw(gizmosRenderer, Gizmos::Gizmo::DrawPhase::Id, mvp);
        }

        if (gizmo.decreaseLifespan(deltaTime.value))
        {
            toRemove.push_back(i);
        }
    }
    while (!toRemove.empty())
    {
        std::size_t i = toRemove.back();
        toRemove.pop_back();
        gizmosVector[i] = gizmosVector.back();
        gizmosVector.pop_back();
    }
}

static void splitViewport(glm::ivec2 position, glm::ivec2 size, int count, BaseRenderer::Viewport* viewports)
{
    if (count == 1)
    {
        viewports[0].position = position;
        viewports[0].size = size;
    }
    else if (count >= 2)
    {
        glm::ivec2 splitSize;
        glm::ivec2 splitOffset;

        // Split along the largest axis.
        if (size.x > size.y)
        {
            splitSize = {size.x / 2, size.y};
            splitOffset = {size.x / 2, 0};
        }
        else
        {
            splitSize = {size.x, size.y / 2};
            splitOffset = {0, size.y / 2};
        }

        splitViewport(position, splitSize, count / 2, viewports);
        splitViewport(position + splitOffset, splitSize, (count + 1) / 2, &viewports[count / 2]);
    }
}

static std::vector<std::pair<glm::mat4, BaseRenderer::Viewport>> getWorldInfo(
    Query<const LocalToWorld&, const Camera&>& query, const ActiveCameras& activeCameras, const glm::ivec2& screenSize)
{
    int cameraCount = 0;

    for (int i = 0; i < 4; ++i) // NOLINT(modernize-loop-convert)
    {
        if (!activeCameras.entities[i].isNull())
        {
            cameraCount += 1;
        }
    }

    BaseRenderer::Viewport viewports[4]{};

    splitViewport({0, 0}, screenSize, cameraCount, viewports);

    std::vector<std::pair<glm::mat4, BaseRenderer::Viewport>> output;

    int usedCount = 0;
    for (int i = 0; i < 4; ++i) // NOLINT(modernize-loop-convert)
    {
        if (activeCameras.entities[i].isNull())
        {
            continue;
        }

        if (auto components = query.at(activeCameras.entities[i]))
        {
            auto& [localToWorld, camera] = *components;
            auto v = glm::inverse(localToWorld.mat);
            auto p = glm::perspective(glm::radians(camera.fovY),
                                      float(viewports[usedCount].size.x) / float(viewports[usedCount].size.y),
                                      camera.zNear, camera.zFar);

            output.emplace_back(p * v, viewports[usedCount]);
            usedCount += 1;
        }
    }

    return output;
}

static std::vector<std::pair<glm::mat4, BaseRenderer::Viewport>> getViewInfo(const ActiveCameras& activeCameras,
                                                                             const glm::ivec2& screenSize)
{
    auto v = glm::translate(glm::mat4(1.0F), glm::vec3(-1.0F, -1.0F, 0.0F));
    auto p = glm::ortho(-0.5F, 0.5F, -0.5F, 0.5F);
    auto vp = v * p;

    int cameraCount = 0;

    for (int i = 0; i < 4; ++i) // NOLINT(modernize-loop-convert)
    {
        if (activeCameras.entities[i].isNull())
        {
            continue;
        }
        cameraCount += 1;
    }

    BaseRenderer::Viewport viewports[4]{};

    splitViewport({0, 0}, screenSize, cameraCount, viewports);

    std::vector<std::pair<glm::mat4, BaseRenderer::Viewport>> output;

    int usedCount = 0;
    for (int i = 0; i < 4; ++i) // NOLINT(modernize-loop-convert)
    {
        if (activeCameras.entities[i].isNull())
        {
            continue;
        }
        output.emplace_back(vp, viewports[usedCount]);
        usedCount += 1;
    }

    return output;
}

static std::vector<std::pair<glm::mat4, BaseRenderer::Viewport>> getScreenInfo(const glm::ivec2& screenSize)
{
    auto v = glm::translate(glm::mat4(1.0F), glm::vec3(-1.0F, -1.0F, 0.0F));
    auto p = glm::ortho(-0.5F, 0.5F, -0.5F, 0.5F);
    auto vp = v * p;

    BaseRenderer::Viewport viewport = {{0, 0}, {screenSize[0], screenSize[1]}};
    std::pair<glm::mat4, BaseRenderer::Viewport> pair = {vp, viewport};
    return {pair};
}

void cubos::engine::gizmosPlugin(Cubos& cubos)
{

    cubos.addPlugin(cubos::engine::rendererPlugin);
    cubos.addPlugin(cubos::engine::screenPickerPlugin);

    cubos.addResource<Gizmos>();
    cubos.addResource<GizmosRenderer>();

    cubos.startupSystem("initialize GizmosRenderer")
        .tagged(GizmosInitTag)
        .after(WindowInitTag)
        .call(
            [](GizmosRenderer& gizmosRenderer, const Window& window) { gizmosRenderer.init(&window->renderDevice()); });

    cubos.system("process gizmos input")
        .tagged(GizmosInputTag)
        .after(WindowPollTag)
        .before(GizmosDrawTag)
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

    cubos.system("draw gizmos")
        .tagged(GizmosDrawTag)
        .after(RendererDrawTag)
        .before(WindowRenderTag)
        .call([](Gizmos& gizmos, GizmosRenderer& gizmosRenderer, ScreenPicker& screenPicker,
                 const ActiveCameras& activeCameras, const Window& window, const DeltaTime& deltaTime,
                 Query<const LocalToWorld&, const Camera&> query) {
            auto screenSize = window->framebufferSize();

            gizmosRenderer.renderDevice->setShaderPipeline(gizmosRenderer.drawPipeline);
            gizmosRenderer.renderDevice->setDepthStencilState(gizmosRenderer.doDepthCheckStencilState);
            gizmosRenderer.renderDevice->clearDepth(1.0F);

            auto worldInfo = getWorldInfo(query, activeCameras, screenSize);
            iterateGizmos(gizmos.worldGizmos, worldInfo, gizmosRenderer, screenPicker, deltaTime);

            gizmosRenderer.renderDevice->setDepthStencilState(gizmosRenderer.noDepthCheckStencilState);
            auto viewInfo = getViewInfo(activeCameras, screenSize);
            iterateGizmos(gizmos.viewGizmos, viewInfo, gizmosRenderer, screenPicker, deltaTime);

            auto screenInfo = getScreenInfo(screenSize);
            iterateGizmos(gizmos.screenGizmos, screenInfo, gizmosRenderer, screenPicker, deltaTime);
        });

    cubos.system("do gizmos screen picking")
        .tagged(GizmosPickTag)
        .after(GizmosDrawTag)
        .call([](GizmosRenderer& gizmosRenderer, Gizmos& gizmos, const ScreenPicker& screenPicker) {
            int mouseX = gizmosRenderer.lastMousePosition.x;
            int mouseY = gizmosRenderer.lastMousePosition.y;

            uint32_t id = screenPicker.at(mouseX, mouseY);
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
        });
}
