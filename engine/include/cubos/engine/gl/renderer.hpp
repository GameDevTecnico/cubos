#ifndef CUBOS_ENGINE_GL_RENDERER_HPP
#define CUBOS_ENGINE_GL_RENDERER_HPP

#include <vector>
#include <functional>
#include <list>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/camera_data.hpp>
#include <cubos/core/gl/light.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/engine/gl/pps/pass.hpp>

namespace cubos::engine::gl
{
    class PostProcessingPass;

    class Renderer
    {
    public:
        using ModelID = size_t;
        using PaletteID = size_t;

    protected:
        struct RendererModel
        {
            core::gl::VertexArray va;
            core::gl::IndexBuffer ib;
            size_t numIndices;
        };

        struct RegisterModelRequest
        {
            const std::vector<core::gl::Vertex>& vertices;
            const std::vector<uint32_t>& indices;
        };

        struct DrawRequest
        {
            ModelID modelId;
            glm::mat4 modelMat;
        };

        std::vector<RendererModel> models;
        size_t modelCounter = 0;
        std::vector<core::gl::ConstantBuffer> palettes;
        core::gl::ConstantBuffer currentPalette;
        std::vector<RegisterModelRequest> registerRequests;
        std::vector<DrawRequest> drawRequests;
        std::vector<core::gl::SpotLight> spotLightRequests;
        std::vector<core::gl::DirectionalLight> directionalLightRequests;
        std::vector<core::gl::PointLight> pointLightRequests;
        core::io::Window& window;
        core::gl::RenderDevice& renderDevice;

        // Post Processing
        core::gl::Framebuffer outputFramebuffer1, outputFramebuffer2;
        core::gl::Texture2D outputTexture1, outputTexture2;

        std::list<std::reference_wrapper<const PostProcessingPass>> postProcessingPasses;

    protected:
        explicit Renderer(core::io::Window& window);
        virtual RendererModel registerModelInternal(const std::vector<core::gl::Vertex>& vertices,
                                                    const std::vector<uint32_t>& indices,
                                                    core::gl::ShaderPipeline pipeline);

        virtual void executePostProcessing(core::gl::Framebuffer target);

    public:
        virtual ~Renderer() = default;
        Renderer(const Renderer&) = delete;

        virtual ModelID registerModel(const std::vector<core::gl::Vertex>& vertices,
                                      const std::vector<uint32_t>& indices) = 0;
        virtual PaletteID registerPalette(const core::gl::Palette& palette);
        virtual void setPalette(PaletteID paletteID);
        virtual void addPostProcessingPass(const PostProcessingPass& pass);
        virtual void getScreenQuad(core::gl::VertexArray& va, core::gl::IndexBuffer& ib) const = 0;
        virtual void render(const core::gl::CameraData& camera, bool usePostProcessing = true) = 0;
        virtual void drawModel(ModelID modelID, glm::mat4 modelMat);
        virtual void drawLight(const core::gl::SpotLight& light);
        virtual void drawLight(const core::gl::DirectionalLight& light);
        virtual void drawLight(const core::gl::PointLight& light);
        virtual void flush();
    };
} // namespace cubos::engine::gl

#endif // CUBOS_ENGINE_GL_RENDERER_HPP