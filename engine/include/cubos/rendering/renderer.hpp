#ifndef CUBOS_RENDERING_RENDERER_HPP
#define CUBOS_RENDERING_RENDERER_HPP

#include <vector>

#include <cubos/gl/render_device.hpp>
#include <cubos/gl/vertex.hpp>
#include <cubos/gl/palette.hpp>
#include <cubos/gl/camera_data.hpp>
#include <cubos/gl/light.hpp>
#include <cubos/io/window.hpp>

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
        std::vector<cubos::gl::ConstantBuffer> palettes;
        cubos::gl::ConstantBuffer currentPalette;
        std::vector<DrawRequest> drawRequests;
        std::vector<gl::SpotLightData> spotLightRequests;
        std::vector<gl::DirectionalLightData> directionalLightRequests;
        std::vector<gl::PointLightData> pointLightRequests;
        io::Window& window;
        gl::RenderDevice& renderDevice;

    public:
        using ModelID = size_t;
        using PaletteID = size_t;

    protected:
        explicit Renderer(io::Window& window);
        virtual ModelID registerModelInternal(const std::vector<cubos::gl::Vertex>& vertices,
                                              std::vector<uint32_t>& indices, cubos::gl::ShaderPipeline pipeline);

    public:
        virtual ~Renderer() = default;
        Renderer(const Renderer&) = delete;

        virtual ModelID registerModel(const std::vector<cubos::gl::Vertex>& vertices,
                                      std::vector<uint32_t>& indices) = 0;
        virtual PaletteID registerPalette(const cubos::gl::Palette& palette);
        virtual void setPalette(PaletteID paletteID);
        virtual void render(const cubos::gl::CameraData& camera, bool usePostProcessing = true) = 0;
        virtual void drawModel(ModelID modelID, glm::mat4 modelMat);
        virtual void drawLight(const cubos::gl::SpotLightData& light);
        virtual void drawLight(const cubos::gl::DirectionalLightData& light);
        virtual void drawLight(const cubos::gl::PointLightData& light);
        virtual void flush();
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_RENDERER_HPP