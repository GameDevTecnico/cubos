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
        {{0, 0, 0}, {0, 0, -1}, 1},
        {{1, 0, 0}, {0, 0, -1}, 1},
        {{1, 1, 0}, {0, 0, -1}, 1},
        {{0, 1, 0}, {0, 0, -1}, 1},
        // Back
        {{1, 0, 1}, {0, 0, 1}, 1},
        {{0, 0, 1}, {0, 0, 1}, 1},
        {{0, 1, 1}, {0, 0, 1}, 1},
        {{1, 1, 1}, {0, 0, 1}, 1},
        // Left
        {{0, 0, 1}, {-1, 0, 0}, 1},
        {{0, 1, 0}, {-1, 0, 0}, 1},
        {{0, 1, 1}, {-1, 0, 0}, 1},
        {{0, 0, 0}, {-1, 0, 0}, 1},
        // Right
        {{1, 0, 0}, {1, 0, 0}, 1},
        {{1, 0, 1}, {1, 0, 0}, 1},
        {{1, 1, 1}, {1, 0, 0}, 1},
        {{1, 1, 0}, {1, 0, 0}, 1},
        // Bottom
        {{1, 0, 0}, {0, -1, 0}, 1},
        {{0, 0, 0}, {0, -1, 0}, 1},
        {{0, 0, 1}, {0, -1, 0}, 1},
        {{1, 0, 1}, {0, -1, 0}, 1},
        // Top
        {{0, 1, 0}, {0, 1, 0}, 1},
        {{1, 1, 0}, {0, 1, 0}, 1},
        {{1, 1, 1}, {0, 1, 0}, 1},
        {{0, 1, 1}, {0, 1, 0}, 1},
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
        glm::lookAt(glm::vec3{5, 5, 5}, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}),
        // glm::mat4 perspectiveMatrix;
        glm::perspective(glm::radians(20.0f), windowSize.y / windowSize.x, 0.1f, 8.f),
        // gl::Framebuffer target;
        0};

    while (!window->shouldClose())
    {
        renderDevice.setFramebuffer(0);
        renderDevice.clearColor(0.0, 0.0, 0.0, 0.0f);

        renderer.drawModel(id, glm::mat4());
        renderer.render(mainCamera, false);
        renderer.flush();

        window->swapBuffers();
        window->pollEvents();
    }

    delete window;
    return 0;
}
