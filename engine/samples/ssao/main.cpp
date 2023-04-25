#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/grid.hpp>
#include <cubos/engine/data/palette.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>
#include <cubos/engine/gl/frame.hpp>

using namespace cubos;

int main()
{
    core::initializeLogger();
    auto window = core::io::openWindow();
    auto& renderDevice = window->getRenderDevice();

    {
        using namespace core::gl;
        using namespace engine::gl;
        using namespace engine;

        // Enable SSAO
        auto settings = core::Settings();
        settings.setBool("ssaoEnabled", true);
        auto renderer = deferred::Renderer(renderDevice, window->getFramebufferSize(), settings);
        auto frame = Frame();

        // Setup asset manager
        core::data::FileSystem::mount("/assets",
                                      std::make_shared<core::data::STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));
        data::AssetManager assetManager = data::AssetManager();
        assetManager.registerType<data::Grid>(&renderer);
        assetManager.registerType<data::Palette>();
        assetManager.importMeta(core::data::FileSystem::find("/assets/"));

        // Load scene
        data::Asset<data::Palette> pal = assetManager.load<data::Palette>("cubos-pal");
        data::Asset<data::Grid> grd = assetManager.load<data::Grid>("cubos");

        auto gpuCube = renderer.upload(grd->grid);
        renderer.setPalette(pal->palette);

        // Initialize the camera.
        Camera camera;
        camera.fovY = 20.0f;
        camera.zNear = 0.1f;
        camera.zFar = 1000.0f;
        camera.view = glm::lookAt(glm::vec3{80, 20, 80}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});

        while (!window->shouldClose())
        {
            while (auto event = window->pollEvent())
            {
                if (std::holds_alternative<core::io::ResizeEvent>(event.value()))
                {
                    auto resizeEvent = std::get<core::io::ResizeEvent>(event.value());
                    renderer.resize(resizeEvent.size);
                }
            }

            auto time = static_cast<float>(window->getTime());

            // Clear the frame commands.
            frame.clear();

            // Draw cube of cubes to the frame.
            auto modelMat = glm::mat4(1.0f);
            frame.draw(gpuCube, modelMat);

            // Add lights to the frame.
            auto spotRotation = glm::quat(glm::vec3(0, time, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
            auto directionalRotation = glm::quat(glm::vec3(0, 90, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
            auto pointRotation = glm::quat(glm::vec3(0, -time, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));

            frame.ambient(glm::vec3(1.1f));
            frame.light(SpotLight(spotRotation * glm::vec3(0, 0, -5), spotRotation, glm::vec3(1), 1, 100,
                                  glm::radians(10.0f), glm::radians(9.0f)));
            frame.light(DirectionalLight(directionalRotation, glm::vec3(1), 0.5f));
            frame.light(PointLight(pointRotation * glm::vec3(0, 0, -2), glm::vec3(1), 1, 10));

            // Render the frame.
            renderer.render(camera, frame);
            window->swapBuffers();
        }
    }

    return 0;
}
