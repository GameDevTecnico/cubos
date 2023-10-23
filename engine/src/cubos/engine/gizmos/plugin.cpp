#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "renderer.hpp"

using cubos::core::ecs::Entity;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using cubos::core::io::Window;

using cubos::engine::ActiveCameras;
using cubos::engine::BaseRenderer;
using cubos::engine::Camera;
using cubos::engine::DeltaTime;
using cubos::engine::Gizmos;
using cubos::engine::GizmosRenderer;
using cubos::engine::LocalToWorld;

static void iterateGizmos(std::vector<std::shared_ptr<Gizmos::Gizmo>>& gizmosVector,
                          const std::vector<std::pair<glm::mat4, BaseRenderer::Viewport>>& cameras,
                          GizmosRenderer& gizmosRenderer, const DeltaTime& deltaTime)
{
    std::vector<std::size_t> toRemove;
    for (std::size_t i = 0; i < gizmosVector.size(); i++)
    {
        auto& gizmo = *(gizmosVector[i]);
        for (const auto& [mvp, viewport] : cameras)
        {
            gizmosRenderer.renderDevice->setViewport(viewport.position.x, viewport.position.y, viewport.size.x,
                                                     viewport.size.y);
            gizmo.draw(gizmosRenderer, mvp);
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
    Query<Read<LocalToWorld>, Read<Camera>>& query, const ActiveCameras& activeCameras, const glm::ivec2& screenSize)
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

        if (auto components = query[activeCameras.entities[i]])
        {
            auto [localToWorld, camera] = *components;
            auto v = glm::inverse(localToWorld->mat);
            auto p = glm::perspective(glm::radians(camera->fovY),
                                      float(viewports[usedCount].size.x) / float(viewports[usedCount].size.y),
                                      camera->zNear, camera->zFar);

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

static void drawGizmosSystem(Write<Gizmos> gizmos, Write<GizmosRenderer> gizmosRenderer,
                             Read<ActiveCameras> activeCameras, Read<Window> window, Read<DeltaTime> deltaTime,
                             Query<Read<LocalToWorld>, Read<Camera>> query)
{
    auto screenSize = (*window)->framebufferSize();

    auto worldInfo = getWorldInfo(query, *activeCameras, screenSize);
    iterateGizmos(gizmos->worldGizmos, worldInfo, *gizmosRenderer, *deltaTime);

    auto viewInfo = getViewInfo(*activeCameras, screenSize);
    iterateGizmos(gizmos->viewGizmos, viewInfo, *gizmosRenderer, *deltaTime);

    auto screenInfo = getScreenInfo(screenSize);
    iterateGizmos(gizmos->screenGizmos, screenInfo, *gizmosRenderer, *deltaTime);
}

static void initGizmosSystem(Write<GizmosRenderer> gizmosRenderer, Read<Window> window)
{
    gizmosRenderer->init(&(*window)->renderDevice());
}

void cubos::engine::gizmosPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.addResource<Gizmos>();
    cubos.addResource<GizmosRenderer>();

    cubos.startupSystem(initGizmosSystem).tagged("cubos.gizmos.init").after("cubos.window.init");
    cubos.system(drawGizmosSystem)
        .tagged("cubos.gizmos.draw")
        .after("cubos.renderer.draw")
        .before("cubos.window.render");
}
