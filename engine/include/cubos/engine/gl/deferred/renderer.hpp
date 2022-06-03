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

        virtual RendererGrid upload(const core::gl::Grid& grid) override;
        virtual void setPalette(const core::gl::Palette& palette) override;

    protected:
        // Implement interface methods.

        virtual void onResize(glm::uvec2 size) override;
        virtual void onRender(const core::gl::Camera& camera, const Frame& frame,
                              core::gl::Framebuffer target) override;

    private:
<<<<<<< HEAD
=======
        // Uploaded grids.

        struct GpuGrid
        {
            core::gl::VertexArray va;
            core::gl::IndexBuffer ib;
            size_t indexCount;
        };

        std::vector<GpuGrid> grids;

>>>>>>> 65f91ba (Fix deferred renderer)
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
        core::gl::Sampler sampler;
        core::gl::Texture2D paletteTex;
        core::gl::ConstantBuffer lightsBuffer;

        // Screen quad used for the lighting pass.

        core::gl::VertexArray screenQuadVA;
    };
} // namespace cubos::engine::gl::deferred

#endif // CUBOS_ENGINE_GL_DEFERRED_RENDERER_HPP
