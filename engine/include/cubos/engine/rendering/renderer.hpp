#ifndef CUBOS_ENGINE_RENDERING_RENDERER_HPP
#define CUBOS_ENGINE_RENDERING_RENDERER_HPP

#include <vector>
#include <functional>
#include <list>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/camera_data.hpp>
#include <cubos/core/gl/light.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/engine/rendering/post_processing/post_processing_pass.hpp>

namespace cubos::engine::rendering
{
    class PostProcessingPass;

    class Renderer
    {
    protected:
        struct RendererModel
        {
            core::gl::VertexArray va;
            core::gl::IndexBuffer ib;
            size_t numIndices;
        };

        struct DrawRequest
        {
            RendererModel& model;
            glm::mat4 modelMat;
        };

        std::vector<RendererModel> models;
        std::vector<core::gl::ConstantBuffer> palettes;
        core::gl::ConstantBuffer currentPalette;
        std::vector<DrawRequest> drawRequests;
        std::vector<core::gl::SpotLightData> spotLightRequests;
        std::vector<core::gl::DirectionalLightData> directionalLightRequests;
        std::vector<core::gl::PointLightData> pointLightRequests;
        core::io::Window& window;
        core::gl::RenderDevice& renderDevice;

        // Post Processing
        core::gl::Framebuffer outputFramebuffer1, outputFramebuffer2;
        core::gl::Texture2D outputTexture1, outputTexture2;

        std::list<std::reference_wrapper<const PostProcessingPass>> postProcessingPasses;

    public:
        using ModelID = size_t;
        using PaletteID = size_t;

    protected:
        explicit Renderer(core::io::Window& window);
        virtual ModelID registerModelInternal(const std::vector<core::gl::Vertex>& vertices,
                                              std::vector<uint32_t>& indices, core::gl::ShaderPipeline pipeline);

        virtual void executePostProcessing(core::gl::Framebuffer target);

    public:
        virtual ~Renderer() = default;
        Renderer(const Renderer&) = delete;

        virtual ModelID registerModel(const std::vector<core::gl::Vertex>& vertices,
                                      std::vector<uint32_t>& indices) = 0;
        virtual PaletteID registerPalette(const core::gl::Palette& palette);
        virtual void setPalette(PaletteID paletteID);
        virtual void addPostProcessingPass(const PostProcessingPass& pass);
        virtual void getScreenQuad(core::gl::VertexArray& va, core::gl::IndexBuffer& ib) const = 0;
        virtual void render(const core::gl::CameraData& camera, bool usePostProcessing = true) = 0;
        virtual void drawModel(ModelID modelID, glm::mat4 modelMat);
        virtual void drawLight(const core::gl::SpotLightData& light);
        virtual void drawLight(const core::gl::DirectionalLightData& light);
        virtual void drawLight(const core::gl::PointLightData& light);
        virtual void flush();
    };
} // namespace cubos::engine::rendering

#endif // CUBOS_ENGINE_RENDERING_RENDERER_HPP