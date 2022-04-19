#include <cubos/core/log.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>
#include "cubos/core/gl/vertex.hpp"
#include "cubos/engine/gl/pps/copy_pass.hpp"
#include <cubos/core/gl/palette.hpp>
#include <cubos/engine/gl/deferred/renderer.hpp>
#include <cubos/engine/gl/pps/copy_pass.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace cubos::core;
using namespace cubos::engine;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    using namespace cubos::core::gl;
    using namespace cubos::engine::gl;

    auto renderer = cubos::engine::gl::deferred::Renderer(*window);

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

    std::vector<Vertex> vertices = {
        // Front
        {{1, 0, 0}, {0, 0, -1}, 1},
        {{0, 0, 0}, {0, 0, -1}, 1},
        {{0, 1, 0}, {0, 0, -1}, 1},
        {{1, 1, 0}, {0, 0, -1}, 1},
        // Back
        {{0, 0, 1}, {0, 0, 1}, 2},
        {{1, 0, 1}, {0, 0, 1}, 2},
        {{1, 1, 1}, {0, 0, 1}, 2},
        {{0, 1, 1}, {0, 0, 1}, 2},
        // Left
        {{1, 0, 1}, {1, 0, 0}, 3},
        {{1, 0, 0}, {1, 0, 0}, 3},
        {{1, 1, 0}, {1, 0, 0}, 3},
        {{1, 1, 1}, {1, 0, 0}, 3},
        // Right
        {{0, 0, 0}, {-1, 0, 0}, 4},
        {{0, 0, 1}, {-1, 0, 0}, 4},
        {{0, 1, 1}, {-1, 0, 0}, 4},
        {{0, 1, 0}, {-1, 0, 0}, 4},
        // Bottom
        {{1, 0, 1}, {0, -1, 0}, 5},
        {{0, 0, 1}, {0, -1, 0}, 5},
        {{0, 0, 0}, {0, -1, 0}, 5},
        {{1, 0, 0}, {0, -1, 0}, 5},
        // Top
        {{1, 1, 0}, {0, 1, 0}, 6},
        {{0, 1, 0}, {0, 1, 0}, 6},
        {{0, 1, 1}, {0, 1, 0}, 6},
        {{1, 1, 1}, {0, 1, 0}, 6},
    };

    std::vector<uint32_t> indices = {
        // Front
        0,
        1,
        3,
        1,
        2,
        3,

        // Back
        4,
        5,
        7,
        5,
        6,
        7,

        // Left
        8,
        9,
        11,
        9,
        10,
        11,

        // Right
        12,
        13,
        15,
        13,
        14,
        15,

        // Bottom
        16,
        17,
        19,
        17,
        18,
        19,

        // Top
        20,
        21,
        23,
        21,
        22,
        23,
    };

    Renderer::ModelID id = renderer.registerModel(vertices, indices);

    CopyPass pass = CopyPass(*window);
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
                                    glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));
                    renderer.drawModel(id, modelMat);
                }
            }
        }

        glm::quat spotLightRotation = glm::quat(glm::vec3(0, t, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
        glm::quat directionalLightRotation =
            glm::quat(glm::vec3(0, 90, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
        glm::quat pointLightRotation = glm::quat(glm::vec3(0, -t, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));

        renderer.drawLight(SpotLight(spotLightRotation * glm::vec3(0, 0, -5), spotLightRotation, glm::vec3(1), 1, 100,
                                     glm::radians(10.0f), glm::radians(9.0f)));
        renderer.drawLight(DirectionalLight(directionalLightRotation, glm::vec3(1), 0.5f));
        renderer.drawLight(PointLight(pointLightRotation * glm::vec3(0, 0, -2), glm::vec3(1), 1, 10));

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

    delete window;
    return 0;
}
