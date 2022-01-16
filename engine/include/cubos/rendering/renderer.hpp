#ifndef CUBOS_RENDERING_RENDERER_HPP
#define CUBOS_RENDERING_RENDERER_HPP

#include <vector>

#include <cubos/gl/render_device.hpp>
#include "cubos/io/window.hpp"

namespace cubos::rendering
{
    class Renderer
    {
    protected:
        struct RendererModel
        {
            cubos::gl::VertexArray va;
            cubos::gl::IndexBuffer ib;
            size_t numIndices;
        };

        struct DrawRequest
        {
            RendererModel& model;
            glm::mat4 modelMat;
        };

        std::vector<RendererModel> models;
        std::vector<DrawRequest> drawRequests;
        io::Window& window;
        gl::RenderDevice& renderDevice;

    protected:
        explicit Renderer(io::Window& window);

    public:
        virtual ~Renderer() = default;
        Renderer(const Renderer&) = delete;
        using ID = size_t;

        virtual ID registerModel(const std::vector<glm::uvec3>& vertices, const std::vector<glm::vec3>& normals,
                                 const std::vector<uint32_t>& materials, std::vector<uint32_t>& indices) = 0;
        virtual void render() = 0;
        virtual void drawModel(ID modelID, glm::mat4 modelMat) = 0;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_RENDERER_HPP