#ifndef CUBOS_RENDERING_RENDERER_HPP
#define CUBOS_RENDERING_RENDERER_HPP

#include <vector>
#include <functional>
#include <list>

#include <cubos/gl/render_device.hpp>
#include <cubos/gl/vertex.hpp>
#include <cubos/gl/palette.hpp>
#include <cubos/gl/camera_data.hpp>
#include <cubos/gl/light.hpp>
#include <cubos/io/window.hpp>
#include <cubos/rendering/post_processing/post_processing_pass.hpp>

namespace cubos::rendering
{
    class PostProcessingPass;

    class Renderer
    {
    public:
        struct RendererModel
        {
            gl::VertexArray va;
            gl::IndexBuffer ib;
            size_t numIndices;
        };

        struct DrawRequest
        {
            RendererModel& model;
            glm::mat4 modelMat;
        };

    protected:
        std::vector<RendererModel> models;
        std::vector<gl::ConstantBuffer> palettes;
        cubos::gl::ConstantBuffer currentPalette;
        std::vector<DrawRequest> drawRequests;
        std::vector<gl::SpotLightData> spotLightRequests;
        std::vector<gl::DirectionalLightData> directionalLightRequests;
        std::vector<gl::PointLightData> pointLightRequests;
        io::Window& window;
        gl::RenderDevice& renderDevice;

        // Post Processing
        gl::Framebuffer outputFramebuffer1, outputFramebuffer2;
        gl::Texture2D outputTexture1, outputTexture2;

        std::list<std::reference_wrapper<const PostProcessingPass>> postProcessingPasses;

    public:
        using ModelID = size_t;
        using PaletteID = size_t;

    protected:
        explicit Renderer(io::Window& window);
        virtual ModelID registerModelInternal(const std::vector<gl::Vertex>& vertices, std::vector<uint32_t>& indices,
                                              gl::ShaderPipeline pipeline);

        virtual void executePostProcessing(gl::Framebuffer target);

    public:
        virtual ~Renderer() = default;
        Renderer(const Renderer&) = delete;

        virtual ModelID registerModel(const std::vector<gl::Vertex>& vertices, std::vector<uint32_t>& indices) = 0;
        virtual PaletteID registerPalette(const gl::Palette& palette);
        virtual void setPalette(PaletteID paletteID);
        virtual void addPostProcessingPass(const PostProcessingPass& pass);
        virtual void getScreenQuad(gl::VertexArray& va, gl::IndexBuffer& ib) const = 0;
        virtual void render(const gl::CameraData& camera, bool usePostProcessing = true) = 0;
        virtual void drawModel(ModelID modelID, glm::mat4 modelMat);
        virtual void drawLight(const gl::SpotLightData& light);
        virtual void drawLight(const gl::DirectionalLightData& light);
        virtual void drawLight(const gl::PointLightData& light);
        virtual void flush();
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_RENDERER_HPP