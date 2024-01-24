#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/ecs/system/query.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/renderer/deferred_renderer.hpp>
#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/frame.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp>
#include <cubos/engine/renderer/pps/bloom.hpp>
#include <cubos/engine/renderer/spot_light.hpp>
#include <cubos/engine/renderer/viewport.hpp>
#include <cubos/engine/screenpicker/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::ResizeEvent;
using cubos::core::io::Window;
using cubos::core::io::WindowEvent;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(RenderableGrid)
{
    return core::ecs::TypeBuilder<RenderableGrid>("cubos::engine::RenderableGrid")
        .withField("asset", &RenderableGrid::asset)
        .withField("offset", &RenderableGrid::offset)
        .build();
}

static void init(Renderer& renderer, const Window& window, Settings& settings)
{
    auto& renderDevice = window->renderDevice();
    renderer = std::make_shared<DeferredRenderer>(renderDevice, window->framebufferSize(), settings);

    if (settings.getBool("cubos.renderer.bloom.enabled", false))
    {
        renderer->pps().addPass<PostProcessingBloom>();
    }
}

static void resize(Renderer& renderer, EventReader<WindowEvent> evs)
{
    for (const auto& ev : evs)
    {
        if (const auto* resizeEv = std::get_if<ResizeEvent>(&ev))
        {
            renderer->resize(resizeEv->size);
        }
    }
}

static void frameGrids(const Assets& assets, Renderer& renderer, RendererFrame& frame,
                       Query<Entity, RenderableGrid&, const LocalToWorld&> query)
{
    for (auto [entity, grid, localToWorld] : query)
    {
        if (grid.handle == nullptr || assets.update(grid.asset))
        {
            // If the grid wasn't already uploaded, we need to upload it now.
            grid.asset = assets.load(grid.asset);
            auto gridRead = assets.read(grid.asset);
            grid.handle = renderer->upload(gridRead.get());
        }

        CUBOS_ASSERT(entity.index < (1 << 31), "Entity index must be lower than 2^31");
        frame.draw(grid.handle, localToWorld.mat * glm::translate(glm::mat4(1.0F), grid.offset), entity.index);
    }
}

static void frameSpotLights(RendererFrame& frame, Query<const SpotLight&, const LocalToWorld&> query)
{
    for (auto [light, localToWorld] : query)
    {
        frame.light(localToWorld.mat, light);
    }
}

static void frameDirectionalLights(RendererFrame& frame, Query<const DirectionalLight&, const LocalToWorld&> query)
{
    for (auto [light, localToWorld] : query)
    {
        frame.light(localToWorld.mat, light);
    }
}

static void framePointLights(RendererFrame& frame, Query<const PointLight&, const LocalToWorld&> query)
{
    for (auto [light, localToWorld] : query)
    {
        frame.light(localToWorld.mat, light);
    }
}

static void frameEnvironment(RendererFrame& frame, const RendererEnvironment& env)
{
    frame.ambient(env.ambient);
    frame.skyGradient(env.skyGradient[0], env.skyGradient[1]);
}

static void checkPaletteUpdateSystem(Assets& assets, Renderer& renderer, ActiveVoxelPalette& activePalette)
{
    if (activePalette.asset.isNull())
    {
        return;
    }

    if (assets.update(activePalette.asset) || activePalette.prev != activePalette.asset)
    {
        auto paletteRead = assets.read(activePalette.asset);
        renderer->setPalette(*paletteRead);
        activePalette.prev = activePalette.asset;
    }
}

static void draw(Renderer& renderer, const ActiveCameras& activeCameras, RendererFrame& frame,
                 Query<const LocalToWorld&, const Camera&, Opt<const Viewport&>> query, ScreenPicker& screenPicker)
{
    Camera cameras[4]{};
    glm::mat4 views[4]{};
    BaseRenderer::Viewport viewports[4]{};

    int cameraCount = 0;

    for (int i = 0; i < 4; ++i) // NOLINT(modernize-loop-convert)
    {
        if (activeCameras.entities[i].isNull())
        {
            continue;
        }

        if (auto components = query.at(activeCameras.entities[i]))
        {
            auto [localToWorld, camera, viewport] = *components;
            cameras[cameraCount].fovY = camera.fovY;
            cameras[cameraCount].zNear = camera.zNear;
            cameras[cameraCount].zFar = camera.zFar;
            if (viewport)
            {
                viewports[i].position = viewport->position;
                viewports[i].size = viewport->size;
            }
            else
            {
                viewports[i].position = {0, 0};
                viewports[i].size = renderer->size();
            }
            views[cameraCount] = glm::inverse(localToWorld.mat);
            cameraCount += 1;
        }
    }

    if (cameraCount == 0)
    {
        CUBOS_WARN("No active camera set - renderer skipping frame");
    }

    for (int i = 0; i < cameraCount; ++i)
    {
        renderer->render(views[i], viewports[i], cameras[i], frame, screenPicker.framebuffer());
    }

    frame.clear();
}

void cubos::engine::rendererPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);
    cubos.addPlugin(windowPlugin);
    cubos.addPlugin(assetsPlugin);
    cubos.addPlugin(screenPickerPlugin);

    cubos.addResource<RendererFrame>();
    cubos.addResource<Renderer>();
    cubos.addResource<ActiveCameras>();
    cubos.addResource<RendererEnvironment>();
    cubos.addResource<ActiveVoxelPalette>();

    cubos.addComponent<RenderableGrid>();
    cubos.addComponent<Camera>();
    cubos.addComponent<SpotLight>();
    cubos.addComponent<DirectionalLight>();
    cubos.addComponent<PointLight>();
    cubos.addComponent<Viewport>();

    cubos.startupTag("cubos.renderer.init").after("cubos.window.init");
    cubos.tag("cubos.renderer.frame").after("cubos.transform.update");
    cubos.tag("cubos.renderer.draw").after("cubos.renderer.frame").before("cubos.window.render");

    cubos.startupSystem(init).tagged("cubos.renderer.init");
    cubos.system(frameGrids).tagged("cubos.renderer.frame");
    cubos.system(frameSpotLights).tagged("cubos.renderer.frame");
    cubos.system(frameDirectionalLights).tagged("cubos.renderer.frame");
    cubos.system(framePointLights).tagged("cubos.renderer.frame");
    cubos.system(frameEnvironment).tagged("cubos.renderer.frame");
    cubos.system(checkPaletteUpdateSystem).tagged("cubos.renderer.frame");
    cubos.system(draw).tagged("cubos.renderer.draw").after("cubos.screenpicker.clear");
    cubos.system(resize).after("cubos.window.poll").before("cubos.renderer.draw");
}
