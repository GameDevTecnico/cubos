#include <cubos/core/ecs/reflection.hpp>
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
#include <cubos/engine/screen_picker/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::rendererInitTag);
CUBOS_DEFINE_TAG(cubos::engine::rendererFrameTag);
CUBOS_DEFINE_TAG(cubos::engine::rendererDrawTag);

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

CUBOS_REFLECT_IMPL(ActiveCameras)
{
    return core::ecs::TypeBuilder<ActiveCameras>("cubos::engine::ActiveCameras").build();
}

CUBOS_REFLECT_IMPL(ActiveVoxelPalette)
{
    return core::ecs::TypeBuilder<ActiveVoxelPalette>("cubos::engine::ActiveVoxelPalette").build();
}

void cubos::engine::rendererPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(screenPickerPlugin);
    cubos.depends(settingsPlugin);

    cubos.resource<RendererFrame>();
    cubos.resource<Renderer>();
    cubos.resource<ActiveCameras>();
    cubos.resource<RendererEnvironment>();
    cubos.resource<ActiveVoxelPalette>();

    cubos.component<RenderableGrid>();
    cubos.component<Camera>();
    cubos.component<SpotLight>();
    cubos.component<DirectionalLight>();
    cubos.component<PointLight>();
    cubos.component<Viewport>();

    cubos.startupTag(rendererInitTag).after(windowInitTag);
    cubos.tag(rendererFrameTag).after(transformUpdateTag);
    cubos.tag(rendererDrawTag).after(rendererFrameTag).before(windowRenderTag);

    cubos.startupSystem("initialize Renderer")
        .tagged(rendererInitTag)
        .call([](Renderer& renderer, const Window& window, Settings& settings) {
            auto& renderDevice = window->renderDevice();
            renderer = std::make_shared<DeferredRenderer>(renderDevice, window->framebufferSize(), settings);

            if (settings.getBool("cubos.renderer.bloom.enabled", false))
            {
                renderer->pps().addPass<PostProcessingBloom>();
            }
        });

    cubos.system("add grids to Frame")
        .tagged(rendererFrameTag)
        .call([](const Assets& assets, Renderer& renderer, RendererFrame& frame,
                 Query<Entity, RenderableGrid&, const LocalToWorld&> query) {
            for (auto [entity, grid, localToWorld] : query)
            {
                if (grid.asset.isNull())
                {
                    // If a grid has no asset, we can't draw it.
                    continue;
                }

                if (grid.handle == nullptr || assets.update(grid.asset))
                {
                    // If the grid wasn't already uploaded, we need to upload it now.
                    grid.asset = assets.load(grid.asset);
                    auto gridRead = assets.read(grid.asset);
                    grid.handle = renderer->upload(gridRead.get());
                }

                CUBOS_ASSERT(entity.index < static_cast<uint32_t>(1 << 31), "Entity index must be lower than 2^31");
                frame.draw(grid.handle, localToWorld.mat * glm::translate(glm::mat4(1.0F), grid.offset), entity.index);
            }
        });

    cubos.system("add spot lights to Frame")
        .tagged(rendererFrameTag)
        .call([](RendererFrame& frame, Query<const SpotLight&, const LocalToWorld&> query) {
            for (auto [light, localToWorld] : query)
            {
                frame.light(localToWorld.mat, light);
            }
        });

    cubos.system("add directional lights to Frame")
        .tagged(rendererFrameTag)
        .call([](RendererFrame& frame, Query<const DirectionalLight&, const LocalToWorld&> query) {
            for (auto [light, localToWorld] : query)
            {
                frame.light(localToWorld.mat, light);
            }
        });

    cubos.system("add point lights to Frame")
        .tagged(rendererFrameTag)
        .call([](RendererFrame& frame, Query<const PointLight&, const LocalToWorld&> query) {
            for (auto [light, localToWorld] : query)
            {
                frame.light(localToWorld.mat, light);
            }
        });

    cubos.system("set Frame environment")
        .tagged(rendererFrameTag)
        .call([](RendererFrame& frame, const RendererEnvironment& env) {
            frame.ambient(env.ambient);
            frame.skyGradient(env.skyGradient[0], env.skyGradient[1]);
        });

    cubos.system("update Palette if changed")
        .tagged(rendererFrameTag)
        .call([](Assets& assets, Renderer& renderer, ActiveVoxelPalette& activePalette) {
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
        });

    cubos.system("draw Frame")
        .tagged(rendererDrawTag)
        .tagged(screenPickerDrawTag)
        .call([](Renderer& renderer, const ActiveCameras& activeCameras, RendererFrame& frame,
                 Query<const LocalToWorld&, const Camera&, Opt<const Viewport&>> query, ScreenPicker& screenPicker,
                 Settings& settings) {
            Camera cameras[4]{};
            glm::mat4 views[4]{};
            BaseRenderer::Viewport viewports[4]{};

            bool screenPickingEnabled = settings.getBool("cubos.renderer.screenPicking.enabled", true);

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
                renderer->render(views[i], viewports[i], cameras[i], frame,
                                 screenPickingEnabled ? screenPicker.framebuffer() : nullptr);
            }

            frame.clear();
        });

    cubos.system("update Renderer on resize")
        .after(windowPollTag)
        .before(rendererDrawTag)
        .call([](Renderer& renderer, EventReader<WindowEvent> evs) {
            for (const auto& ev : evs)
            {
                if (const auto* resizeEv = std::get_if<ResizeEvent>(&ev))
                {
                    renderer->resize(resizeEv->size);
                }
            }
        });
}
