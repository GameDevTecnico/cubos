#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/gl/vertex.hpp>
#include <cubos/gl/palette.hpp>
#include <cubos/rendering/deferred/deferred_renderer.hpp>
#include <cubos/rendering/shadow_mapping/csm_shadow_mapper.hpp>
#include <cubos/rendering/post_processing/copy_pass.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <chrono>
#include <thread>

using namespace cubos;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();

    auto shadowMapper = rendering::CSMShadowMapper(renderDevice, 512, 1024, 256, 4);
    shadowMapper.setCascadeDistances({5, 10, 20});

    auto renderer = rendering::DeferredRenderer(*window);

    renderer.setShadowMapper(&shadowMapper);

    gl::Palette palette1(std::vector<gl::Material>{
        {{1, 0, 0, 1}},
        {{0, 1, 0, 1}},
        {{0, 0, 1, 1}},
        {{1, 1, 0, 1}},
        {{0, 1, 1, 1}},
        {{1, 0, 1, 1}},
    });
    gl::Palette palette2(std::vector<gl::Material>{
        {{0, 1, 1, 1}},
        {{1, 0, 1, 1}},
        {{1, 1, 0, 1}},
        {{0, 0, 1, 1}},
        {{1, 0, 0, 1}},
        {{0, 1, 0, 1}},
    });

    auto palette1ID = renderer.registerPalette(palette1);
    auto palette2ID = renderer.registerPalette(palette2);

    std::vector<cubos::gl::Vertex> vertices = {
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

    rendering::Renderer::ModelID id = renderer.registerModel(vertices, indices);

    rendering::CopyPass pass = rendering::CopyPass(*window);
    renderer.addPostProcessingPass(pass);

    glm::vec2 windowSize = window->getFramebufferSize();
    gl::CameraData mainCamera = {glm::lookAt(glm::vec3{7, 7, 7}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}),
                                 glm::radians(20.0f),
                                 windowSize.x / windowSize.y,
                                 0.1f,
                                 100.f,
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

        glm::quat directionalLightRotation =
            glm::quat(glm::vec3(0, t * 0.5f, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
        glm::quat pointLightRotation =
            glm::quat(glm::vec3(0, -t * 0.1f, 0)) * glm::quat(glm::vec3(glm::radians(30.0f), 0, 0));

        /*/
        glm::quat spotLightRotation =
            glm::quat(glm::vec3(0, -t * 0.5f, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
        renderer.drawLight(gl::SpotLightData(spotLightRotation * glm::vec3(0, 0, -5), spotLightRotation, glm::vec3(1),
                                             1, 20, glm::radians(10.0f), glm::radians(9.0f), false));
        spotLightRotation = glm::quat(glm::vec3(0, t * 0.5f, 0)) * glm::quat(glm::vec3(glm::radians(45.0f), 0, 0));
        renderer.drawLight(gl::SpotLightData(spotLightRotation * glm::vec3(0, 0, -5), spotLightRotation, glm::vec3(1),
                                             1, 20, glm::radians(10.0f), glm::radians(9.0f), false));
        /**/

        /*/
        renderer.drawLight(gl::DirectionalLightData(directionalLightRotation, glm::vec3(1), 1.0f, false));
        /**/

        /**/
        renderer.drawLight(gl::PointLightData(pointLightRotation * glm::vec3(0, 0, -2), glm::vec3(1), 1, 10, false));
        /**/

        if (sin(t * 4) > 0)
        {
            renderer.setPalette(palette1ID);
        }
        else
        {
            renderer.setPalette(palette2ID);
        }

        renderer.render(mainCamera, false);
        renderer.flush();

        window->swapBuffers();
        window->pollEvents();
    }

    delete window;
    return 0;
}
