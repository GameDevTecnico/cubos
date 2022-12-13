#include <cubos/engine/plugins/renderer.hpp>
#include <cubos/engine/plugins/transform.hpp>

#include <cubos/engine/ecs/draw_system.hpp>

#include <cubos/engine/ecs/grid.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>

#include <cubos/core/settings.hpp>

static void startup(gl::Renderer& renderer, const cubos::core::io::Window& window)
{
    auto& renderDevice = window->getRenderDevice();
    renderer =
        std::make_shared<gl::deferred::Renderer>(renderDevice, window->getFramebufferSize(), cubos::core::Settings());
}

void cubos::engine::plugins::rendererPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin)
        .addComponent<ecs::Grid>()
        .addResource<gl::Frame>()
        .addResource<gl::Renderer>()

        .addStartupSystem(startup, "SetRenderer")
        .addSystem(ecs::drawSystem, "Draw");
}