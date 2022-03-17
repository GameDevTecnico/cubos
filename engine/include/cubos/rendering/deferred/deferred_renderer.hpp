#ifndef CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP
#define CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP

#include <vector>

#include <cubos/gl/vertex.hpp>
#include <cubos/gl/render_device.hpp>
#include <cubos/rendering/renderer.hpp>
#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

#define CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT 128

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
                SpotLightData(const gl::SpotLightData& light);
            } spotLights[CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT];

            struct DirectionalLightData
            {
                glm::mat4 rotation;
                glm::vec4 color;
                float intensity;
                float padding[3];

                DirectionalLightData() = default;
                DirectionalLightData(const gl::DirectionalLightData& light);
            } directionalLights[CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT];

            struct PointLightData
            {
                glm::vec4 position;
                glm::vec4 color;
                float intensity;
                float range;
                float padding[2];

                PointLightData() = default;
                PointLightData(const gl::PointLightData& light);
            } pointLights[CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT];
            uint32_t numSpotLights = 0;
            uint32_t numDirectionalLights = 0;
            uint32_t numPointLights = 0;
        } lights;

        struct CameraDataUniform
        {
            glm::mat4 V;
            float nearPlane;
            float farPlane;
            float padding[2];
        };

        struct ShadowMapInfoUniform
        {
            glm::mat4 spotMatrices[CUBOS_MAX_SPOT_SHADOW_MAPS];
            glm::mat4 directionalMatrices[CUBOS_MAX_DIRECTIONAL_SHADOW_MAPS];
            size_t numSpotShadows;
            size_t numDirectionalShadows;
            size_t numPointShadows;
            float padding;
        };

        struct CascadeInfoUniform
        {
            float cascadeDistances[CUBOS_MAX_DIRECTIONAL_SHADOW_MAP_STRIDE];
            uint32_t cascadeCount;
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
        gl::Texture2D materialTex;
        gl::Texture2D depthTex;
        // endregion

        // region outputBuffer
        // Shader Pipeline
        gl::ShaderPipeline outputPipeline;
        gl::ShaderBindingPoint outputPositionBP;
        gl::ShaderBindingPoint outputNormalBP;
        gl::ShaderBindingPoint outputMaterialBP;
        gl::ShaderBindingPoint outputPaletteBP;
        gl::ShaderBindingPoint outputLightBlockBP;
        gl::ConstantBuffer outputLightBlockBuffer;

        gl::ShaderBindingPoint cameraDataBP;
        gl::ConstantBuffer cameraDataBuffer;

        gl::ShaderBindingPoint shadowMapInfoBP;
        gl::ConstantBuffer shadowMapInfoBuffer;

        gl::ShaderBindingPoint cascadeInfoBP;
        gl::ConstantBuffer cascadeInfoBuffer;

        gl::ShaderBindingPoint spotShadowMapBP;
        gl::ShaderBindingPoint directionalShadowMapBP;
        gl::ShaderBindingPoint pointShadowMapBP;

        // Screen Quad
        gl::VertexArray screenVertexArray;
        gl::IndexBuffer screenIndexBuffer;

        // Samplers
        gl::Sampler positionSampler;
        gl::Sampler normalSampler;
        gl::Sampler materialSampler;

        gl::Sampler shadowMapSampler;
        // endregion

        ShadowMapper* shadowMapper = nullptr;

    private:
        void createShaderPipelines();
        void setupFrameBuffers();

        void createRenderDeviceStates();

    public:
        explicit DeferredRenderer(io::Window& window);
        virtual void getScreenQuad(cubos::gl::VertexArray& va, cubos::gl::IndexBuffer& ib) const override;
        virtual ModelID registerModel(const std::vector<cubos::gl::Vertex>& vertices,
                                      std::vector<uint32_t>& indices) override;
        virtual void drawLight(const cubos::gl::SpotLightData& light) override;
        virtual void drawLight(const cubos::gl::DirectionalLightData& light) override;
        virtual void drawLight(const cubos::gl::PointLightData& light) override;
        void setShadowMapper(ShadowMapper* shadowMapper);
        virtual void render(const cubos::gl::CameraData& camera, bool usePostProcessing = true) override;
        virtual void flush() override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_DEFERRED_DEFERRED_RENDERER_HPP