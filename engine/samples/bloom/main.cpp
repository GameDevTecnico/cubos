#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/renderer/deferred/renderer.hpp>
#include <cubos/engine/renderer/frame.hpp>
#include <cubos/engine/renderer/pps/bloom.hpp>

using namespace cubos;

int main()
{
    core::initializeLogger();
    auto window = core::io::openWindow();
    auto& renderDevice = window->getRenderDevice();

    {
        using namespace core::gl;
        using namespace engine::gl;

        auto renderer = deferred::Renderer(renderDevice, window->getFramebufferSize(), core::Settings());
        auto frame = Frame();

        // Set the palette.
        Palette palette({{{2, 0, 0, 1}},
                         {{0, 2, 0, 1}},
                         {{0, 0, 2, 1}},
                         {{2, 2, 0, 1}},
                         {{2, 0, 2, 1}},
                         {{0, 2, 2, 1}},
                         {{2, 2, 2, 1}}});
        renderer.setPalette(palette);

        // Create and upload a simple grid.
        Grid cube({16, 1, 16});
        uint16_t paletteIdx = 0;
        for (int x = 0; x < 16; x++)
        {
            for (int z = 0; z < 16; z++)
            {
                cube.set({x, 0, z}, paletteIdx + 1);
                paletteIdx = static_cast<uint16_t>((paletteIdx + 1) % 7);
            }
        }
        auto gpuCube = renderer.upload(cube);

        // Add a bloom pass to the post processing stack.
        renderer.pps().addPass<pps::BloomPass>();

        // Initialize the camera.
        Camera camera;
        camera.fovY = 20.0f;
        camera.zNear = 0.1f;
        camera.zFar = 1000.0f;
        camera.view = glm::lookAt(glm::vec3{7, 7, 7}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});

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
            auto modelMat =
                glm::translate(glm::mat4(1.0f), glm::vec3(-4, 0, -4)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
            frame.draw(gpuCube, modelMat);

            // Add lights to the frame.
            auto spotRotation = glm::quat(glm::vec3(0, time, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
            auto directionalRotation = glm::quat(glm::vec3(0, 90, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
            auto pointRotation = glm::quat(glm::vec3(0, -time, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));

            frame.ambient(glm::vec3(0.1f));
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
