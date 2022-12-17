#include <cubos/engine/plugins/renderer.hpp>
#include <cubos/engine/plugins/transform.hpp>

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

static void draw(gl::Renderer& renderer, const plugins::ActiveCamera& camera, const gl::Frame& frame)
{
    // renderer->render(camera, frame);
}

void cubos::engine::plugins::rendererPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin)
        .addComponent<ecs::Grid>()
        .addComponent<ecs::Camera>()
        .addResource<gl::Frame>()
        .addResource<gl::Renderer>()
        .addResource<ActiveCamera>()
        .addStartupSystem(startup, "SetRenderer")
        .addSystem(ecs::drawSystem, "CreateDrawList")
        .addSystem(draw, "Draw");
}