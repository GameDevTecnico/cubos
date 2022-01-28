#ifndef CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP
#define CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP

#include <vector>

#include <cubos/gl/vertex.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/rendering/renderer.hpp>

namespace cubos::rendering
{
    class DeferredRenderer : public Renderer
    {
    private:
        struct MVP
        {
            glm::mat4 M;
            glm::mat4 V;
            glm::mat4 P;
        };

        // region gBuffer
        //  Shader Pipeline
        gl::ShaderPipeline gBufferPipeline;
        gl::ShaderBindingPoint mvpBP;
        gl::ConstantBuffer mvpBuffer;
        gl::RasterState rasterState;
        gl::BlendState blendState;
        gl::DepthStencilState depthStencilState;

        // Framebuffers
        gl::Framebuffer gBuffer;

        // Textures
        gl::Texture2D positionTex;
        gl::Texture2D normalTex;
        gl::Texture2D colorTex;
        gl::Texture2D depthTex;
        // endregion

        // region outputBuffer
        // Shader Pipeline
        gl::ShaderPipeline smallOutputPipeline;
        gl::ShaderBindingPoint smallOutputPositionBP;
        gl::ShaderBindingPoint smallOutputNormalBP;
        gl::ShaderBindingPoint smallOutputMaterialBP;
        gl::ShaderBindingPoint smallOutputPaletteBP;
        gl::ShaderPipeline largeOutputPipeline;
        gl::ShaderBindingPoint largeOutputPositionBP;
        gl::ShaderBindingPoint largeOutputNormalBP;
        gl::ShaderBindingPoint largeOutputMaterialBP;
        gl::ShaderBindingPoint largeOutputPaletteBP;
        gl::VertexArray screenVertexArray;
        gl::IndexBuffer screenIndexBuffer;
        // Samplers
        gl::Sampler positionSampler;
        gl::Sampler normalSampler;
        gl::Sampler materialSampler;
        // endregion
    private:
        void createShaderPipelines();
        void setupFrameBuffers();

        void createRenderDeviceStates();

    public:
        explicit DeferredRenderer(io::Window& window);
        ModelID registerModel(const std::vector<cubos::gl::Vertex>& vertices, std::vector<uint32_t>& indices) override;
        virtual PaletteID registerPalette(const cubos::gl::Palette& palette) override;
        virtual void setPalette(PaletteID paletteID) override;
        void drawModel(ModelID modelID, glm::mat4 modelMat) override;
        void render(const CameraData& camera, bool usePostProcessing = true) override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP