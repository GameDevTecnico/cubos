#include <cubos/core/log.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/grid.hpp>
#include <cubos/engine/data/palette.hpp>
#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace cubos;
using namespace engine;
using namespace core::data;

int main(int argc, char** argv)
{
    core::initializeLogger();
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    // Open the window.
    auto window = std::unique_ptr<core::io::Window>(core::io::Window::create());
    auto& renderDevice = window->getRenderDevice();

    using namespace core::gl;
    using namespace engine::gl;

    // Initialize the renderer.
    auto renderer = deferred::Renderer(renderDevice, window->getFramebufferSize());
    auto frame = Frame();

    // Initialize the asset manager and register the asset types.
    data::AssetManager assetManager;
    assetManager.registerType<data::Grid>(&renderer);
    assetManager.registerType<data::Palette>();

    // Import all asset meta datdirectorys in the assets directory.
    assetManager.importMeta(FileSystem::find("/assets/"));

    // Loads all unloaded static assets.
    assetManager.loadStatic();

    // Set the palette.
    auto paletteAsset = assetManager.load<data::Palette>("palette");
    renderer.setPalette(paletteAsset->palette);

    // Set the grid.
    auto car = assetManager.load<data::Grid>("car");

    // Initialize the camera.
    Camera camera;
    camera.fovY = 20.0f;
    camera.zNear = 0.1f;
    camera.zFar = 1000.0f;
    camera.view = glm::lookAt(glm::vec3{10, 10, 10}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});

    window->onFramebufferResize.registerCallback([&](glm::ivec2 sz) { renderer.resize(sz); });
    while (!window->shouldClose())
    {
        window->pollEvents();

        // Clear the frame commands.
        frame.clear();

        // Add the car to the frame.
        frame.draw(car->id, glm::scale(glm::mat4(1.0f), {0.1f, 0.1f, 0.1f}));

        // Add lights to the frame.
        auto directionalRotation = glm::quat(glm::vec3(0, 90, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
        frame.ambient(glm::vec3(0.5f));
        frame.light(DirectionalLight(directionalRotation, glm::vec3(1), 0.5f));

        // Render the frame.
        renderer.render(camera, frame, false);
        window->swapBuffers();
    }

    return 0;
}
