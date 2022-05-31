#ifndef CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP
#define CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP

#include <cubos/engine/gl/renderer.hpp>

#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/render_device.hpp>

#include <vector>

#define CUBOS_DEFERRED_RENDERER_MAX_SPOT_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_DIRECTIONAL_LIGHT_COUNT 128
#define CUBOS_DEFERRED_RENDERER_MAX_POINT_LIGHT_COUNT 128

namespace cubos::engine::gl::deferred
{
    /// Renderer implementation which uses deferred rendering.
    ///
    /// Voxel grids are first triangulated, and then the triangles are uploaded to the GPU.
    /// The rendering is done in two passes:
    /// - The first pass renders the scene to the GBuffer textures: position, normal and material.
    /// - The second pass takes the GBuffer textures and calculates the color of the pixels with the lighting applied.
    class Renderer : public gl::Renderer
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        Renderer(core::gl::RenderDevice& renderDevice, glm::uvec2 size);
        virtual ~Renderer() override = default;

        // Implement interface methods.

        virtual GridID upload(const core::gl::Grid& grid) override;
        virtual void free(GridID grid) override;
        virtual void setPalette(const core::gl::Palette& palette) override;

    protected:
        // Implement interface methods.

        virtual void onResize(glm::uvec2 size) override;
        virtual void onRender(const core::gl::Camera& camera, const Frame& frame,
                              core::gl::Framebuffer target) override;

    private:
        void createShaderPipelines();
        void setupFrameBuffers();
        void createRenderDeviceStates();

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

        //  Shader Pipeline.
        core::gl::ShaderPipeline gBufferPipeline;
        core::gl::ShaderBindingPoint mvpBP;
        core::gl::ConstantBuffer mvpBuffer;
        core::gl::RasterState rasterState;
        core::gl::BlendState blendState;
        core::gl::DepthStencilState depthStencilState;

        // Framebuffers
        core::gl::Framebuffer gBuffer;

        // Textures.
        core::gl::Texture2D paletteTex;
        core::gl::Texture2D positionTex;
        core::gl::Texture2D normalTex;
        core::gl::Texture2D materialTex;
        core::gl::Texture2D depthTex;

        // Shader Pipeline.
        core::gl::ShaderPipeline outputPipeline;
        core::gl::ShaderBindingPoint outputPositionBP;
        core::gl::ShaderBindingPoint outputNormalBP;
        core::gl::ShaderBindingPoint outputMaterialBP;
        core::gl::ShaderBindingPoint outputPaletteBP;
        core::gl::ShaderBindingPoint outputLightBlockBP;
        core::gl::ConstantBuffer outputLightBlockBuffer;

        // Screen Quad.
        core::gl::VertexArray screenVertexArray;
        core::gl::IndexBuffer screenIndexBuffer;

        // Samplers.
        core::gl::Sampler positionSampler;
        core::gl::Sampler normalSampler;
        core::gl::Sampler materialSampler;
    };
} // namespace cubos::engine::gl::deferred

#endif // CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP
