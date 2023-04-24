#ifndef CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP
#define CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP

#include <vector>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/gl/renderer.hpp>

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
        virtual ~Renderer() override = default;

        // Implement interface methods.

        virtual RendererGrid upload(const core::gl::Grid& grid) override;
        virtual void setPalette(const core::gl::Palette& palette) override;

    protected:
        // Implement interface methods.

        virtual void onResize(glm::uvec2 size) override;
        virtual void onRender(const core::gl::Camera& camera, const Frame& frame,
                              core::gl::Framebuffer target) override;

    private:
        void createSSAOTextures();
        void generateSSAONoise();

        // GBuffer.

        glm::uvec2 size;
        core::gl::Framebuffer gBuffer;
        core::gl::Texture2D positionTex;
        core::gl::Texture2D normalTex;
        core::gl::Texture2D materialTex;
        core::gl::Texture2D depthTex;

        //  Geometry pass pipeline.

        core::gl::ShaderPipeline geometryPipeline;
        core::gl::ShaderBindingPoint mvpBP;
        core::gl::ConstantBuffer mvpBuffer;
        core::gl::RasterState geometryRasterState;
        core::gl::BlendState geometryBlendState;
        core::gl::DepthStencilState geometryDepthStencilState;

        // Lighting pass pipeline.

        core::gl::ShaderPipeline lightingPipeline;
        core::gl::ShaderBindingPoint paletteBP;
        core::gl::ShaderBindingPoint positionBP;
        core::gl::ShaderBindingPoint normalBP;
        core::gl::ShaderBindingPoint materialBP;
        core::gl::ShaderBindingPoint lightsBP;
        core::gl::ShaderBindingPoint ssaoEnabledBP;
        core::gl::ShaderBindingPoint ssaoTexBP;
        core::gl::Sampler sampler;
        core::gl::Texture2D paletteTex;
        core::gl::ConstantBuffer lightsBuffer;

        // Screen quad used for the lighting pass.

        core::gl::VertexArray screenQuadVA;

        // SSAO (Screen-Scrape Ambient Occlusion)

        bool ssaoEnabled = false;
        std::vector<glm::vec3> ssaoKernel;

        core::gl::Framebuffer ssaoFB;
        core::gl::Texture2D ssaoTex;
        core::gl::Texture2D ssaoNoiseTex;
        core::gl::Sampler ssaoNoiseSampler;

        core::gl::ShaderPipeline ssaoPipeline;
        core::gl::ShaderBindingPoint ssaoPositionBP;
        core::gl::ShaderBindingPoint ssaoNormalBP;
        core::gl::ShaderBindingPoint ssaoNoiseBP;
        core::gl::ShaderBindingPoint ssaoSamplesBP;
        core::gl::ShaderBindingPoint ssaoViewBP;
        core::gl::ShaderBindingPoint ssaoProjectionBP;
        core::gl::ShaderBindingPoint ssaoScreenSizeBP;

        core::gl::ShaderPipeline ssaoBlurPipeline;
        core::gl::ShaderBindingPoint ssaoBlurTexBP;
    };
} // namespace cubos::engine::gl::deferred

#endif // CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP
