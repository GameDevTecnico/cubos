#include <cubos/core/log.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/grid.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>
#include <cubos/engine/gl/pps/copy_pass.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace cubos;
using namespace cubos::core;
using namespace cubos::core::gl;
using namespace cubos::engine;
using namespace cubos::engine::gl;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    {
        auto renderer = deferred::Renderer(*window);

        Palette palette1(std::vector<Material>{
            {{1, 0, 0, 1}},
            {{0, 1, 0, 1}},
            {{0, 0, 1, 1}},
            {{1, 1, 0, 1}},
            {{0, 1, 1, 1}},
            {{1, 0, 1, 1}},
        });
        Palette palette2(std::vector<Material>{
            {{0, 1, 1, 1}},
            {{1, 0, 1, 1}},
            {{1, 1, 0, 1}},
            {{0, 0, 1, 1}},
            {{1, 0, 0, 1}},
            {{0, 1, 0, 1}},
        });

        auto palette1ID = renderer.registerPalette(palette1);
        auto palette2ID = renderer.registerPalette(palette2);

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Grid grid({3, 2, 3});
        grid.set({0, 0, 0}, 1);
        grid.set({1, 0, 0}, 2);
        grid.set({2, 0, 0}, 1);
        grid.set({0, 0, 1}, 2);
        grid.set({1, 0, 1}, 1);
        grid.set({2, 0, 1}, 2);
        grid.set({0, 0, 2}, 1);
        grid.set({1, 0, 2}, 2);
        grid.set({2, 0, 2}, 1);
        grid.set({1, 1, 1}, 3);

        Renderer::ModelID id = renderer.registerModel(grid);

        pps::CopyPass pass = pps::CopyPass(*window);
        renderer.addPostProcessingPass(pass);

        glm::vec2 windowSize = window->getFramebufferSize();
        CameraData mainCamera = {// glm::mat4 viewMatrix;
                                 glm::lookAt(glm::vec3{7, 7, 7}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}),
                                 // glm::mat4 perspectiveMatrix;
                                 glm::perspective(glm::radians(20.0f), windowSize.x / windowSize.y, 0.1f, 1000.f),
                                 // gl::Framebuffer target;
                                 0};
        float t = 0;
        int s = 0;

        while (!window->shouldClose())
        {
            float currentT = window->getTime();
            float deltaT = 0;
            if (t != 0)
            {
                deltaT = currentT - t;
                int newS = std::round(t);
                if (newS != s)
                    logDebug("FPS: {}", std::round(1 / deltaT));
                s = newS;
            }
            t = currentT;
            renderDevice.setFramebuffer(0);
            renderDevice.clearColor(0.0, 0.0, 0.0, 0.0f);

            auto modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)) *
                            glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 1.5f, 0.5f)) *
                            glm::rotate(glm::mat4(1.0f), glm::radians(t) * 32, {0.0f, 1.0f, 0.0});
            renderer.drawModel(id, modelMat);

            glm::quat directionalLightRotation =
                glm::quat(glm::vec3(0, 90, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
            renderer.drawLight(DirectionalLight(directionalLightRotation, glm::vec3(1), 0.5f));

            if (sin(t * 4) > 0)
            {
                renderer.setPalette(palette1ID);
            }
            else
            {
                renderer.setPalette(palette2ID);
            }

            renderer.render(mainCamera);
            renderer.flush();

            window->swapBuffers();
            window->pollEvents();
        }
    }

    delete window;
    return 0;
}
