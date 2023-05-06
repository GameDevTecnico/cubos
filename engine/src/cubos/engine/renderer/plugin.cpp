#include <components/cubos/camera.hpp>
#include <components/cubos/grid.hpp>
#include <components/cubos/local_to_world.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cubos/core/ecs/query.hpp>
#include <cubos/core/gl/camera.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/ecs/camera.hpp>
#include <cubos/engine/ecs/grid.hpp>
#include <cubos/engine/transform/local_to_world.hpp>

static void init(gl::Renderer& renderer, const cubos::core::io::Window& window, const cubos::core::Settings& settings)
{
    auto& renderDevice = window->getRenderDevice();
    renderer = std::make_shared<gl::deferred::Renderer>(renderDevice, window->getFramebufferSize(), settings);
}

static void frame(
    gl::Frame& frame,
    cubos::core::ecs::Query<const cubos::engine::ecs::Grid&, const cubos::engine::ecs::LocalToWorld&> query)
{
    for (auto [entity, grid, localToWorld] : query)
    {
        frame.draw(grid.handle->rendererGrid, localToWorld.mat * glm::translate(glm::mat4(1.0f), grid.modelOffset));
    }
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

void cubos::engine::rendererPlugin(Cubos& cubos)
{
    cubos.addPlugin(transformPlugin);
    cubos.addPlugin(windowPlugin);

    cubos.addResource<gl::Frame>();
    cubos.addResource<gl::Renderer>();
    cubos.addResource<ActiveCamera>();

    cubos.addComponent<ecs::Grid>();
    cubos.addComponent<ecs::Camera>();

    cubos.startupTag("cubos.renderer.init").afterTag("cubos.window.init");
    cubos.tag("cubos.renderer.frame").afterTag("cubos.transform.update");
    cubos.tag("cubos.renderer.render").afterTag("cubos.renderer.frame").beforeTag("cubos.window.render");

    cubos.startupSystem(init).tagged("cubos.renderer.init");
    cubos.system(frame).tagged("cubos.renderer.frame");
    cubos.system(draw).tagged("cubos.renderer.draw");
}
