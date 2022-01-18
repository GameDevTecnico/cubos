#ifndef CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP
#define CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP

#include <vector>

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
        gl::ShaderBindingPoint mvpBindingPoint;
        gl::ConstantBuffer mvpBuffer;

        // Framebuffers
        gl::Framebuffer gBuffer;

        // Textures
        gl::Texture2D positionTex;
        gl::Texture2D normalTex;
        gl::Texture2D materialTex;
        gl::Texture2D depthTex;
        // endregion

        // region outputBuffer
        // Shader Pipeline
        gl::ShaderPipeline outputPipeline;
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

    public:
        explicit DeferredRenderer(io::Window& window);
        ID registerModel(const std::vector<VertexModel>& vertices, std::vector<uint32_t>& indices) override;
        void drawModel(ID modelID, glm::mat4 modelMat) override;

        void render(const CameraData& camera, bool usePostProcessing = true) override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP