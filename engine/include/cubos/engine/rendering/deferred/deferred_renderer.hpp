#ifndef CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP
#define CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP

#include <vector>

#include "cubos/core/gl/vertex.hpp"
#include <cubos/core/gl/render_device.hpp>
#include "cubos/engine/rendering/renderer.hpp"

#define CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT 128

namespace cubos::engine::rendering
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

        struct LightBlock
        {
            struct SpotLightData
            {
                glm::vec4 position;
                glm::mat4 rotation;
                glm::vec4 color;
                float intensity;
                float range;
                float spotCutoff;
                float innerSpotCutoff;

                SpotLightData() = default;
                SpotLightData(const core::gl::SpotLightData& light);
            } spotLights[CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT];

            struct DirectionalLightData
            {
                glm::mat4 rotation;
                glm::vec4 color;
                float intensity;
                float padding[3];

                DirectionalLightData() = default;
                DirectionalLightData(const core::gl::DirectionalLightData& light);
            } directionalLights[CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT];

            struct PointLightData
            {
                glm::vec4 position;
                glm::vec4 color;
                float intensity;
                float range;
                float padding[2];

                PointLightData() = default;
                PointLightData(const core::gl::PointLightData& light);
            } pointLights[CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT];
            uint32_t numSpotLights = 0;
            uint32_t numDirectionalLights = 0;
            uint32_t numPointLights = 0;
        } lights;

        // region gBuffer
        //  Shader Pipeline
        core::gl::ShaderPipeline gBufferPipeline;
        core::gl::ShaderBindingPoint mvpBP;
        core::gl::ConstantBuffer mvpBuffer;
        core::gl::RasterState rasterState;
        core::gl::BlendState blendState;
        core::gl::DepthStencilState depthStencilState;

        // Framebuffers
        core::gl::Framebuffer gBuffer;

        // Textures
        core::gl::Texture2D positionTex;
        core::gl::Texture2D normalTex;
        core::gl::Texture2D materialTex;
        core::gl::Texture2D depthTex;
        // endregion

        // region outputBuffer
        // Shader Pipeline
        core::gl::ShaderPipeline outputPipeline;
        core::gl::ShaderBindingPoint outputPositionBP;
        core::gl::ShaderBindingPoint outputNormalBP;
        core::gl::ShaderBindingPoint outputMaterialBP;
        core::gl::ShaderBindingPoint outputPaletteBP;
        core::gl::ShaderBindingPoint outputLightBlockBP;
        core::gl::ConstantBuffer outputLightBlockBuffer;

        // Screen Quad
        core::gl::VertexArray screenVertexArray;
        core::gl::IndexBuffer screenIndexBuffer;

        // Samplers
        core::gl::Sampler positionSampler;
        core::gl::Sampler normalSampler;
        core::gl::Sampler materialSampler;
        // endregion
    private:
        void createShaderPipelines();
        void setupFrameBuffers();

        void createRenderDeviceStates();

    public:
        explicit DeferredRenderer(core::io::Window& window);
        virtual void getScreenQuad(core::gl::VertexArray& va, core::gl::IndexBuffer& ib) const override;
        virtual ModelID registerModel(const std::vector<core::gl::Vertex>& vertices,
                                      std::vector<uint32_t>& indices) override;
        virtual void drawLight(const core::gl::SpotLightData& light) override;
        virtual void drawLight(const core::gl::DirectionalLightData& light) override;
        virtual void drawLight(const core::gl::PointLightData& light) override;
        virtual void render(const core::gl::CameraData& camera, bool usePostProcessing = true) override;
        virtual void flush() override;
    };
} // namespace cubos::engine::rendering

#endif // CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP