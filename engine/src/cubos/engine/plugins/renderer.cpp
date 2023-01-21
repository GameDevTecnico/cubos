#include <cubos/engine/plugins/renderer.hpp>
#include <cubos/engine/plugins/transform.hpp>
#include <cubos/engine/plugins/window.hpp>

#include <cubos/engine/ecs/draw_system.hpp>
#include <components/cubos/camera.hpp>
#include <components/cubos/grid.hpp>

#include <cubos/engine/gl/deferred/renderer.hpp>
#include <cubos/engine/gl/renderer.hpp>
#include <cubos/engine/gl/frame.hpp>

#include <cubos/core/gl/camera.hpp>

#include <cubos/core/settings.hpp>

static void startup(gl::Renderer& renderer, const cubos::core::io::Window& window)
{
    auto& renderDevice = window->getRenderDevice();
    renderer =
        std::make_shared<gl::deferred::Renderer>(renderDevice, window->getFramebufferSize(), cubos::core::Settings());
}

static void draw(gl::Renderer& renderer, const plugins::ActiveCamera& activeCamera, gl::Frame& frame,
                 cubos::core::ecs::Query<const ecs::LocalToWorld&, const ecs::Camera&> query)
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

void cubos::engine::plugins::rendererPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin)
        .addPlugin(windowPlugin)
        .addComponent<ecs::Grid>()
        .addComponent<ecs::Camera>()
        .addResource<gl::Frame>()
        .addResource<gl::Renderer>()
        .addResource<ActiveCamera>();

    cubos.startupTag("SetRenderer").afterTag("OpenWindow");
    cubos.startupSystem(startup).tagged("SetRenderer");
    cubos.system(ecs::drawSystem).tagged("CreateDrawList");
    cubos.system(draw).tagged("Draw");
}