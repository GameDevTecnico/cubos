#ifndef CUBOS_ENGINE_GL_RENDERER_HPP
#define CUBOS_ENGINE_GL_RENDERER_HPP

#include <vector>
#include <functional>
#include <list>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/grid.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/camera.hpp>
#include <cubos/core/gl/light.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/gl/pps/pass.hpp>

namespace cubos::engine::gl
{
    namespace pps
    {
        class Pass;
    }

    /// Interface class which abstracts different rendering methods.
    class Renderer
    {
    public:
        using GridID = size_t;
        using PaletteID = size_t;

        virtual ~Renderer() = default;
        Renderer(const Renderer&) = delete;

        /// Uploads a grid to the GPU and returns an ID, which can be used to draw it.
        /// @param grid The grid to upload.
        /// @return The ID of the grid.
        virtual GridID registerGrid(const core::gl::Grid& grid) = 0;

        /// Sets the current palette of the renderer.
        /// @param palette The palette to set.
        virtual void setPalette(const core::gl::Palette& palette) = 0;

        /// Draws a frame.
        /// @param camera The camera to use.
        /// @param frame The frame to draw.
        /// @param usePostProcessing Whether to use post processing.
        virtual void render(const core::gl::Camera& camera, const Frame& frame, bool usePostProcessing = true) = 0;

        virtual void addPostProcessingPass(const pps::Pass& pass);
        virtual void getScreenQuad(core::gl::VertexArray& va, core::gl::IndexBuffer& ib) const = 0;

    protected:
        explicit Renderer(core::io::Window& window);

        virtual void executePostProcessing(core::gl::Framebuffer target);

        core::io::Window& window;
        core::gl::RenderDevice& renderDevice;

        // Post Processing
        core::gl::Framebuffer outputFramebuffer1, outputFramebuffer2;
        core::gl::Texture2D outputTexture1, outputTexture2;
        std::list<std::reference_wrapper<const pps::Pass>> postProcessingPasses;
    };
} // namespace cubos::engine::gl

#endif // CUBOS_ENGINE_GL_RENDERER_HPP
