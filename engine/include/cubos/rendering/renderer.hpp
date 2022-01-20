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

    public:
        struct VertexModel
        {
            glm::uvec3 vertex;
            glm::vec3 normal;
            uint32_t material;
        };

        struct CameraData
        {
            glm::mat4 viewMatrix;
            glm::mat4 perspectiveMatrix;
            gl::Framebuffer target;
        };

    protected:
        explicit Renderer(io::Window& window);

    public:
        virtual ~Renderer() = default;
        Renderer(const Renderer&) = delete;
        using ID = size_t;

        virtual ID registerModel(const std::vector<VertexModel>& vertices, std::vector<uint32_t>& indices) = 0;
        virtual void render(const CameraData& camera, bool usePostProcessing = true) = 0;
        virtual void drawModel(ID modelID, glm::mat4 modelMat) = 0;
        virtual void flush();
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_RENDERER_HPP