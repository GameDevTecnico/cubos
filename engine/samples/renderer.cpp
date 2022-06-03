#include <cubos/core/log.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/palette.hpp>

#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>
#include <cubos/engine/gl/pps/copy_pass.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace cubos;

int main(void)
{
    core::initializeLogger();
    auto window = core::io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    {
        using namespace core::gl;
        using namespace engine::gl;

        auto renderer = deferred::Renderer(renderDevice, window->getFramebufferSize());
        auto frame = Frame();

        // Set the palette.
        Palette palette({
            {{1, 0, 0, 1}},
            {{0, 1, 0, 1}},
            {{0, 0, 1, 1}},
        });
        renderer.setPalette(palette);

        // Create and upload a simple grid.
        Grid cube({2, 2, 2});
        cube.set({0, 0, 0}, 1);
        cube.set({0, 0, 1}, 2);
        cube.set({0, 1, 0}, 3);
        cube.set({0, 1, 1}, 1);
        cube.set({1, 0, 0}, 2);
        cube.set({1, 0, 1}, 3);
        cube.set({1, 1, 0}, 1);
        cube.set({1, 1, 1}, 2);
        auto gpuCube = renderer.upload(cube);

        // Add a copy pass to the post processing stack.
        renderer.pps().addPass<pps::CopyPass>();

        // Initialize the camera.
        Camera camera;
        camera.fovY = 20.0f;
        camera.zNear = 0.1f;
        camera.zFar = 1000.0f;
        camera.view = glm::lookAt(glm::vec3{7, 7, 7}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});

        float t = 0;
        int s = 0;

        window->onFramebufferResize.registerCallback([&](glm::ivec2 sz) { renderer.resize(sz); });
        while (!window->shouldClose())
        {
            window->pollEvents();
            // Calculate the delta time.
            float currentT = window->getTime();
            float deltaT = 0;
            if (t != 0)
            {
                deltaT = currentT - t;
                int newS = std::round(t);
                if (newS != s)
                    core::logDebug("FPS: {}", std::round(1 / deltaT));
                s = newS;
            }
            t = currentT;

            // Clear the frame commands.
            frame.clear();

            // Draw cube of cubes to the frame.
            for (int x = -1; x < 1; x++)
            {
                for (int y = -1; y < 1; y++)
                {
                    for (int z = -1; z < 1; z++)
                    {
                        auto axis = glm::vec3(x, y, z) * 2.0f + glm::vec3(1);

                        auto modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) *
                                        glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)) *
                                        glm::rotate(glm::mat4(1.0f), glm::radians(t), axis) *
                                        glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)) *
                                        glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
                        frame.draw(gpuCube, modelMat);
                    }
                }
            }

            // Add lights to the frame.
            auto spotRotation = glm::quat(glm::vec3(0, t, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
            auto directionalRotation = glm::quat(glm::vec3(0, 90, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
            auto pointRotation = glm::quat(glm::vec3(0, -t, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));

            frame.ambient(glm::vec3(0.1f));
            frame.light(SpotLight(spotRotation * glm::vec3(0, 0, -5), spotRotation, glm::vec3(1), 1, 100,
                                  glm::radians(10.0f), glm::radians(9.0f)));
            frame.light(DirectionalLight(directionalRotation, glm::vec3(1), 0.5f));
            frame.light(PointLight(pointRotation * glm::vec3(0, 0, -2), glm::vec3(1), 1, 10));

            // Render the frame.
            renderer.render(camera, frame, false);
            window->swapBuffers();
        }
    }

    delete window;
    return 0;
}
