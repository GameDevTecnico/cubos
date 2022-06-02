#ifndef CUBOS_ENGINE_GL_RENDERER_HPP
#define CUBOS_ENGINE_GL_RENDERER_HPP

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/grid.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/camera.hpp>
#include <cubos/core/gl/light.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/gl/pps/manager.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::gl
{
    namespace pps
    {
        class Pass;
    }

    class Frame;

    /// Interface class which abstracts different rendering methods.
    /// @details This abstraction allows us to, for example, switch between a
    /// raytracing and a deferred rendering method as we need to, without
    /// changing the API. This is useful since not all computers support
    /// realtime raytracing.
    class Renderer
    {
    public:
        using GridID = size_t;
        using PaletteID = size_t;

        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        Renderer(core::gl::RenderDevice& renderDevice, glm::uvec2 size);
        Renderer(const Renderer&) = delete;
        virtual ~Renderer() = default;

        /// Uploads a grid to the GPU and returns an ID, which can be used to draw it.
        /// @param grid The grid to upload.
        /// @return The ID of the grid.
        virtual GridID upload(const core::gl::Grid& grid) = 0;

        /// Frees a grid from the GPU.
        /// @param grid The ID of the grid to free.
        virtual void free(GridID grid) = 0;

        /// Sets the current palette of the renderer.
        /// @param palette The palette to set.
        virtual void setPalette(const core::gl::Palette& palette) = 0;

        /// Resizes the renderer's framebuffers.
        /// @param size The new size of the window.
        void resize(glm::uvec2 size);

        /// Draws a frame, with post processing.
        /// @param camera The camera to use.
        /// @param frame The frame to draw.
        /// @param usePostProcessing Whether to use post processing.
        /// @param target The target framebuffer to draw to.
        void render(const core::gl::Camera& camera, const Frame& frame, bool usePostProcessing = true,
                    core::gl::Framebuffer target = 0);

        /// Gets a reference to the post processing manager.
        /// @return The post processing manager.
        pps::Manager& pps();

    protected:
        core::gl::RenderDevice& renderDevice; ///< The render device being used.

        /// Called when resizze() is called.
        /// Renderer implementations should override this function to resize their framebuffers.
        virtual void onResize(glm::uvec2 size) = 0;

        /// Called when render() is called, before applying post processing effects.
        /// Renderer implementations should implement this function to draw the frame.
        /// @param camera The camera to use.
        /// @param frame The frame to draw.
        /// @param target The target framebuffer. If postprocessing is enabled, it won't be the camera's target.
        virtual void onRender(const core::gl::Camera& camera, const Frame& frame, core::gl::Framebuffer target) = 0;

    private:
        pps::Manager ppsManager;           ///< The post processing manager.
        core::gl::Framebuffer framebuffer; ///< The framebuffer where the frame is drawn.
        core::gl::Texture2D texture;       ///< The texture where the frame is drawn.
    };
} // namespace cubos::engine::gl

#endif // CUBOS_ENGINE_GL_RENDERER_HPP
