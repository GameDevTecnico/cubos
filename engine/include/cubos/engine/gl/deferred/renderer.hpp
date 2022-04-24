#ifndef CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP
#define CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP

#include <vector>

#include "cubos/core/gl/vertex.hpp"
#include <cubos/core/gl/render_device.hpp>
#include "cubos/engine/gl/renderer.hpp"

#define CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT 128

namespace cubos::engine::gl::deferred
{
    class Renderer : public gl::Renderer
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
                SpotLightData(const core::gl::SpotLight& light);
            } spotLights[CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT];

            struct DirectionalLightData
            {
                glm::mat4 rotation;
                glm::vec4 color;
                float intensity;
                float padding[3];

                DirectionalLightData() = default;
                DirectionalLightData(const core::gl::DirectionalLight& light);
            } directionalLights[CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT];

            struct PointLightData
            {
                glm::vec4 position;
                glm::vec4 color;
                float intensity;
                float range;
                float padding[2];

                PointLightData() = default;
                PointLightData(const core::gl::PointLight& light);
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

    public:
        explicit Renderer(core::io::Window& window);
        virtual void getScreenQuad(core::gl::VertexArray& va, core::gl::IndexBuffer& ib) const override;
        virtual ModelID registerModel(const core::gl::Grid& grid) override;
        virtual void drawLight(const core::gl::SpotLight& light) override;
        virtual void drawLight(const core::gl::DirectionalLight& light) override;
        virtual void drawLight(const core::gl::PointLight& light) override;
        virtual void render(const core::gl::CameraData& camera, bool usePostProcessing = true) override;
        virtual void flush() override;

    private:
        void createShaderPipelines();
        void setupFrameBuffers();

        void createRenderDeviceStates();
    };
} // namespace cubos::engine::gl::deferred

#endif // CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP