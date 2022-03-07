#include <cubos/log.hpp>
#include <cubos/io/window.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/gl/vertex.hpp>
#include <cubos/gl/palette.hpp>
#include <cubos/gl/grid.hpp>
#include <cubos/gl/triangulation.hpp>
#include <cubos/rendering/deferred/deferred_renderer.hpp>
#include <cubos/rendering/post_processing/copy_pass.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>

using namespace cubos;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    auto renderer = rendering::DeferredRenderer(*window);

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

    std::vector<cubos::gl::Vertex> vertices;

    std::vector<uint32_t> indices;

    cubos::gl::Grid grid(glm::ivec3(3, 2, 3));
    grid.set(glm::ivec3(0, 0, 0), 1);
    grid.set(glm::ivec3(0, 0, 1), 1);
    grid.set(glm::ivec3(0, 0, 2), 1);
    grid.set(glm::ivec3(1, 0, 0), 1);
    grid.set(glm::ivec3(1, 0, 1), 1);
    grid.set(glm::ivec3(1, 0, 2), 1);
    grid.set(glm::ivec3(2, 0, 0), 1);
    grid.set(glm::ivec3(2, 0, 1), 1);
    grid.set(glm::ivec3(2, 0, 2), 1);
    grid.set(glm::ivec3(2, 1, 2), 2);

    std::vector<cubos::gl::Triangle> triangles = cubos::gl::Triangulation::Triangulate(grid);

    std::unordered_map<cubos::gl::Vertex, int, cubos::gl::Vertex::hash> vertex_to_index;

    for (auto it = triangles.begin(); it != triangles.end(); it++)
    {
        int v0_index = -1;
        if (!vertex_to_index.contains(it->v0))
        {
            v0_index = vertex_to_index[it->v0] = vertices.size();
            vertices.push_back(it->v0);
        }
        else
        {
            v0_index = vertex_to_index[it->v0];
        }

        indices.push_back(v0_index);

        int v1_index = -1;
        if (!vertex_to_index.contains(it->v1))
        {
            v1_index = vertex_to_index[it->v1] = vertices.size();
            vertices.push_back(it->v1);
        }
        else
        {
            v1_index = vertex_to_index[it->v1];
        }

        indices.push_back(v1_index);

        int v2_index = -1;
        if (!vertex_to_index.contains(it->v2))
        {
            v2_index = vertex_to_index[it->v2] = vertices.size();
            vertices.push_back(it->v2);
        }
        else
        {
            v2_index = vertex_to_index[it->v2];
        }

        indices.push_back(v2_index);
    }

    rendering::Renderer::ModelID id = renderer.registerModel(vertices, indices);

    rendering::CopyPass pass = rendering::CopyPass(*window);
    renderer.addPostProcessingPass(pass);

    glm::vec2 windowSize = window->getFramebufferSize();
    gl::CameraData mainCamera = {// glm::mat4 viewMatrix;
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

        renderer.drawLight(gl::SpotLightData(spotLightRotation * glm::vec3(0, 0, -5), spotLightRotation, glm::vec3(1),
                                             1, 100, glm::radians(10.0f), glm::radians(9.0f)));
        renderer.drawLight(gl::DirectionalLightData(directionalLightRotation, glm::vec3(1), 0.5f));
        renderer.drawLight(gl::PointLightData(pointLightRotation * glm::vec3(0, 0, -2), glm::vec3(1), 1, 10));

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
