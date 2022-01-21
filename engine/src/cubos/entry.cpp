#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/rendering/deferred/deferred_renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace cubos;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    auto renderer = rendering::DeferredRenderer(*window);

    std::vector<rendering::Renderer::VertexModel> vertices = {
        // Front
        {{1, 0, 0}, {0, 0, -1}, 0},
        {{0, 0, 0}, {0, 0, -1}, 0},
        {{0, 1, 0}, {0, 0, -1}, 0},
        {{1, 1, 0}, {0, 0, -1}, 0},
        // Back
        {{0, 0, 1}, {0, 0, 1}, 0xf},
        {{1, 0, 1}, {0, 0, 1}, 0xf},
        {{1, 1, 1}, {0, 0, 1}, 0xf},
        {{0, 1, 1}, {0, 0, 1}, 0xf},
        // Left
        {{1, 0, 1}, {1, 0, 0}, 0xff},
        {{1, 0, 0}, {1, 0, 0}, 0xff},
        {{1, 1, 0}, {1, 0, 0}, 0xff},
        {{1, 1, 1}, {1, 0, 0}, 0xff},
        // Right
        {{0, 0, 0}, {-1, 0, 0}, 0xfff},
        {{0, 0, 1}, {-1, 0, 0}, 0xfff},
        {{0, 1, 1}, {-1, 0, 0}, 0xfff},
        {{0, 1, 0}, {-1, 0, 0}, 0xfff},
        // Bottom
        {{1, 0, 1}, {0, -1, 0}, 1},
        {{0, 0, 1}, {0, -1, 0}, 1},
        {{0, 0, 0}, {0, -1, 0}, 1},
        {{1, 0, 0}, {0, -1, 0}, 1},
        // Top
        {{1, 1, 0}, {0, 1, 0}, 0xffff},
        {{0, 1, 0}, {0, 1, 0}, 0xffff},
        {{0, 1, 1}, {0, 1, 0}, 0xffff},
        {{1, 1, 1}, {0, 1, 0}, 0xffff},
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

    rendering::Renderer::ID id = renderer.registerModel(vertices, indices);

    glm::vec2 windowSize = window->getFramebufferSize();
    rendering::Renderer::CameraData mainCamera = {
        // glm::mat4 viewMatrix;
        glm::lookAt(glm::vec3{7, 7, 7}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}),
        // glm::mat4 perspectiveMatrix;
        glm::perspective(glm::radians(20.0f), windowSize.x / windowSize.y, 0.1f, 1000.f),
        // gl::Framebuffer target;
        0};

    while (!window->shouldClose())
    {
        float t = window->getTime();
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
                                    glm::rotate(glm::mat4(1.0f), glm::radians(t * 10), axis) *
                                    glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));
                    renderer.drawModel(id, modelMat);
                }
            }
        }

        /*
        auto modelMat1 = glm::rotate(glm::mat4(1.0f), glm::radians(t * 10), glm::vec3(0, 1, 0)) *
                         glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));
        auto modelMat2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0, -0.5f)) *
                         glm::rotate(glm::mat4(1.0f), glm::radians(t * 10), glm::vec3(0, 1, 0)) *
                         glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));

        renderer.drawModel(id, modelMat1);
        renderer.drawModel(id, modelMat2);
         */
        renderer.render(mainCamera, false);
        renderer.flush();

        window->swapBuffers();
        window->pollEvents();
    }

    delete window;
    return 0;
}
