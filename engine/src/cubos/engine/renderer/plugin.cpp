#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <cubos/core/ecs/query.hpp>
#include <cubos/core/gl/camera.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/renderer/deferred_renderer.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/frame.hpp>
#include <cubos/engine/renderer/light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/pps/bloom.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::EventReader;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::io::ResizeEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;
using namespace cubos::engine;

static void init(Write<Renderer> renderer, Read<Window> window, Read<Settings> settings)
{
    auto& renderDevice = (*window)->renderDevice();
    *renderer = std::make_shared<DeferredRenderer>(renderDevice, (*window)->framebufferSize(), *settings);
    (*renderer)->pps().addPass<PostProcessingBloom>();
}

static void resize(Write<Renderer> renderer, EventReader<WindowEvent> evs)
{
    for (const auto& ev : evs)
    {
        if (const auto* resizeEv = std::get_if<ResizeEvent>(&ev))
        {
            (*renderer)->resize(resizeEv->size);
        }
    }
}

static void frameGrids(Read<Assets> assets, Write<Renderer> renderer, Write<RendererFrame> frame,
                       Query<Write<RenderableGrid>, Read<LocalToWorld>> query)
{
    for (auto [entity, grid, localToWorld] : query)
    {
        if (grid->handle == nullptr || assets->update(grid->asset))
        {
            // If the grid wasn't already uploaded, we need to upload it now.
            grid->asset = assets->load(grid->asset);
            auto gridRead = assets->read(grid->asset);
            grid->handle = (*renderer)->upload(gridRead.get());
        }

        frame->draw(grid->handle, localToWorld->mat * glm::translate(glm::mat4(1.0F), grid->offset));
    }
}

static void frameSpotLights(Write<RendererFrame> frame, Query<Read<SpotLight>, Read<LocalToWorld>> query)
{
    for (auto [entity, light, localToWorld] : query)
    {
        auto position = localToWorld->mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
        frame->light(cubos::core::gl::SpotLight{
            {position.x, position.y, position.z},
            glm::quat_cast(localToWorld->mat),
            light->color,
            light->intensity,
            light->range,
            light->spotAngle,
            light->innerSpotAngle,
        });
    }
}

static void frameDirectionalLights(Write<RendererFrame> frame, Query<Read<DirectionalLight>, Read<LocalToWorld>> query)
{
    for (auto [entity, light, localToWorld] : query)
    {
        frame->light(cubos::core::gl::DirectionalLight{
            glm::quat_cast(localToWorld->mat),
            light->color,
            light->intensity,
        });
    }
}

static void framePointLights(Write<RendererFrame> frame, Query<Read<PointLight>, Read<LocalToWorld>> query)
{
    for (auto [entity, light, localToWorld] : query)
    {
        auto position = localToWorld->mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
        frame->light(cubos::core::gl::PointLight{
            {position.x, position.y, position.z},
            light->color,
            light->intensity,
            light->range,
        });
    }
}

static void frameEnvironment(Write<RendererFrame> frame, Read<RendererEnvironment> env)
{
    frame->ambient(env->ambient);
    frame->skyGradient(env->skyGradient[0], env->skyGradient[1]);
}

/// @brief Splits the viewport recursively for the given cameras.
/// @param position Viewport position.
/// @param size Viewport size.
/// @param count How many cameras need to be fitted in to the given viewport.
/// @param cameras Output array where the viewports will be set.
static void splitViewport(glm::ivec2 position, glm::ivec2 size, int count, cubos::core::gl::Camera* cameras)
{
    if (count == 1)
    {
        cameras[0].viewportPosition = position;
        cameras[0].viewportSize = size;
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

        splitViewport(position, splitSize, count / 2, cameras);
        splitViewport(position + splitOffset, splitSize, (count + 1) / 2, &cameras[count / 2]);
    }
}

static void draw(Write<Renderer> renderer, Read<ActiveCameras> activeCameras, Write<RendererFrame> frame,
                 Query<Read<LocalToWorld>, Read<Camera>> query)
{
    cubos::core::gl::Camera cameras[4]{};
    int cameraCount = 0;

    for (int i = 0; i < 4; ++i)
    {
        if (activeCameras->entities[i].isNull())
        {
            continue;
        }

        if (auto components = query[activeCameras->entities[i]])
        {
            auto [localToWorld, camera] = *components;
            cameras[cameraCount].fovY = camera->fovY;
            cameras[cameraCount].zNear = camera->zNear;
            cameras[cameraCount].zFar = camera->zFar;
            cameras[cameraCount].view = glm::inverse(localToWorld->mat);
            cameraCount += 1;
        }
    }

    splitViewport({0, 0}, (*renderer)->size(), cameraCount, cameras);

    if (cameraCount == 0)
    {
        CUBOS_WARN("No active camera set - renderer skipping frame");
    }

    for (int i = 0; i < cameraCount; ++i)
    {
        (*renderer)->render(cameras[i], *frame);
    }

    frame->clear();
}

void cubos::engine::rendererPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);
    cubos.addPlugin(windowPlugin);
    cubos.addPlugin(assetsPlugin);

    cubos.addResource<RendererFrame>();
    cubos.addResource<Renderer>();
    cubos.addResource<ActiveCameras>();
    cubos.addResource<RendererEnvironment>();

    cubos.addComponent<RenderableGrid>();
    cubos.addComponent<Camera>();
    cubos.addComponent<SpotLight>();
    cubos.addComponent<DirectionalLight>();
    cubos.addComponent<PointLight>();

    cubos.startupTag("cubos.renderer.init").after("cubos.window.init");
    cubos.tag("cubos.renderer.frame").after("cubos.transform.update");
    cubos.tag("cubos.renderer.render").after("cubos.renderer.frame").before("cubos.window.render");

    cubos.startupSystem(init).tagged("cubos.renderer.init");
    cubos.system(frameGrids).tagged("cubos.renderer.frame");
    cubos.system(frameSpotLights).tagged("cubos.renderer.frame");
    cubos.system(frameDirectionalLights).tagged("cubos.renderer.frame");
    cubos.system(framePointLights).tagged("cubos.renderer.frame");
    cubos.system(frameEnvironment).tagged("cubos.renderer.frame");
    cubos.system(draw).tagged("cubos.renderer.draw");
    cubos.system(resize).after("cubos.window.poll").before("cubos.renderer.draw");
}
