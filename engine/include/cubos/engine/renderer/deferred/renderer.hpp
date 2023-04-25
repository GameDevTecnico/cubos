#pragma once

#include <vector>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/renderer/renderer.hpp>

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
    class Renderer : public gl::BaseRenderer
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        /// @param settings The settings to use.
        Renderer(core::gl::RenderDevice& renderDevice, glm::uvec2 size, const core::Settings& settings);
        ~Renderer() override = default;

        // Implement interface methods.

        RendererGrid upload(const core::gl::Grid& grid) override;
        void setPalette(const core::gl::Palette& palette) override;

    protected:
        // Implement interface methods.

        void onResize(glm::uvec2 size) override;
        void onRender(const core::gl::Camera& camera, const Frame& frame, core::gl::Framebuffer target) override;

    private:
        void createSSAOTextures();
        void generateSSAONoise();

        // GBuffer.

        glm::uvec2 mSize;
        core::gl::Framebuffer mGBuffer;
        core::gl::Texture2D mPositionTex;
        core::gl::Texture2D mNormalTex;
        core::gl::Texture2D mAterialTex;
        core::gl::Texture2D mDepthTex;

        //  Geometry pass pipeline.

        core::gl::ShaderPipeline mGeometryPipeline;
        core::gl::ShaderBindingPoint mVpBp;
        core::gl::ConstantBuffer mVpBuffer;
        core::gl::RasterState mGeometryRasterState;
        core::gl::BlendState mGeometryBlendState;
        core::gl::DepthStencilState mGeometryDepthStencilState;

        // Lighting pass pipeline.

        core::gl::ShaderPipeline mLightingPipeline;
        core::gl::ShaderBindingPoint mPaletteBp;
        core::gl::ShaderBindingPoint mPositionBp;
        core::gl::ShaderBindingPoint mNormalBp;
        core::gl::ShaderBindingPoint mAterialBp;
        core::gl::ShaderBindingPoint mLightsBp;
        core::gl::ShaderBindingPoint mSsaoEnabledBp;
        core::gl::ShaderBindingPoint mSsaoTexBp;
        core::gl::Sampler mSampler;
        core::gl::Texture2D mPaletteTex;
        core::gl::ConstantBuffer mLightsBuffer;

        // Screen quad used for the lighting pass.

        core::gl::VertexArray mScreenQuadVa;

        // SSAO (Screen-Scrape Ambient Occlusion)

        bool mSsaoEnabled = false;
        std::vector<glm::vec3> mSsaoKernel;

        core::gl::Framebuffer mSsaoFb;
        core::gl::Texture2D mSsaoTex;
        core::gl::Texture2D mSsaoNoiseTex;
        core::gl::Sampler mSsaoNoiseSampler;

        core::gl::ShaderPipeline mSsaoPipeline;
        core::gl::ShaderBindingPoint mSsaoPositionBp;
        core::gl::ShaderBindingPoint mSsaoNormalBp;
        core::gl::ShaderBindingPoint mSsaoNoiseBp;
        core::gl::ShaderBindingPoint mSsaoSamplesBp;
        core::gl::ShaderBindingPoint mSsaoViewBp;
        core::gl::ShaderBindingPoint mSsaoProjectionBp;
        core::gl::ShaderBindingPoint mSsaoScreenSizeBp;

        core::gl::ShaderPipeline mSsaoBlurPipeline;
        core::gl::ShaderBindingPoint mSsaoBlurTexBp;
    };
} // namespace cubos::engine::gl::deferred
