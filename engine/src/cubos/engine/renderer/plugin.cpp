#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/ecs/query.hpp>
#include <cubos/core/gl/camera.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/renderer/deferred_renderer.hpp>
#include <cubos/engine/renderer/frame.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::Query;
using cubos::core::io::Window;
using namespace cubos::engine;

static void init(Renderer& renderer, const Window& window, const Settings& settings)
{
    auto& renderDevice = window->getRenderDevice();
    renderer = std::make_shared<DeferredRenderer>(renderDevice, window->getFramebufferSize(), settings);
}

static void frame(RendererFrame& frame, Query<const RenderableGrid&, const LocalToWorld&> query)
{
    (void)frame;
    for (auto [entity, grid, localToWorld] : query)
    {
        (void)entity;
        (void)grid;
        (void)localToWorld;
        // frame.draw(grid.asset->rendererGrid, localToWorld.mat);
    }
}

static void draw(Renderer& renderer, const ActiveCamera& activeCamera, RendererFrame& frame,
                 Query<const LocalToWorld&, const Camera&> query)
{
    cubos::core::gl::Camera glCamera;

    if (auto components = query[activeCamera.entity])
    {
        auto [localToWorld, camera] = *components;
        glCamera.fovY = camera.fovY;
        glCamera.zNear = camera.zNear;
        glCamera.zFar = camera.zFar;
        glCamera.view = glm::inverse(localToWorld.mat);
    }

    renderer->render(glCamera, frame);
    frame.clear();
}

void cubos::engine::rendererPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.addResource<RendererFrame>();
    cubos.addResource<Renderer>();
    cubos.addResource<ActiveCamera>();

    cubos.addComponent<RenderableGrid>();
    cubos.addComponent<Camera>();

    cubos.startupTag("cubos.renderer.init").afterTag("cubos.window.init");
    cubos.tag("cubos.renderer.frame").afterTag("cubos.transform.update");
    cubos.tag("cubos.renderer.render").afterTag("cubos.renderer.frame").beforeTag("cubos.window.render");

    cubos.startupSystem(init).tagged("cubos.renderer.init");
    cubos.system(frame).tagged("cubos.renderer.frame");
    cubos.system(draw).tagged("cubos.renderer.draw");
}
