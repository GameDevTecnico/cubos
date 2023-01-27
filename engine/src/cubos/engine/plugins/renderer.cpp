#include <cubos/engine/plugins/renderer.hpp>
#include <cubos/engine/plugins/transform.hpp>
#include <cubos/engine/plugins/window.hpp>

#include <components/cubos/camera.hpp>
#include <components/cubos/grid.hpp>

#include <cubos/engine/gl/deferred/renderer.hpp>
#include <cubos/engine/gl/renderer.hpp>
#include <cubos/engine/gl/frame.hpp>

#include <cubos/core/gl/camera.hpp>

#include <cubos/core/settings.hpp>

#include <glm/gtc/matrix_transform.hpp>

static void startup(gl::Renderer& renderer, const cubos::core::io::Window& window,
                    const cubos::core::Settings& settings)
{
    auto& renderDevice = window->getRenderDevice();
    renderer = std::make_shared<gl::deferred::Renderer>(renderDevice, window->getFramebufferSize(), settings);
}

static void buildFrame(
    gl::Frame& frame,
    cubos::core::ecs::Query<const cubos::engine::ecs::Grid&, const cubos::engine::ecs::LocalToWorld&> query)
{
    for (auto [entity, grid, localToWorld] : query)
    {
        frame.draw(grid.handle->rendererGrid, localToWorld.mat * glm::translate(glm::mat4(1.0f), grid.modelOffset));
    }
}

static void render(gl::Renderer& renderer, const plugins::ActiveCamera& activeCamera, gl::Frame& frame,
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
    cubos.system(buildFrame).tagged("CreateDrawList").afterTag("Transform");
    cubos.system(render).tagged("Draw").afterTag("CreateDrawList");
}