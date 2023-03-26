#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/camera.hpp>
#include <cubos/core/gl/grid.hpp>
#include <cubos/core/gl/light.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/gl/pps/manager.hpp>

namespace cubos::engine::gl
{
    class Frame;
    class BaseRenderer;

    namespace pps
    {
        class Pass;
    }

    namespace impl
    {
        class RendererGrid;
    }

    /// Handle to a grid uploaded to the GPU, to be used for rendering.
    using RendererGrid = std::shared_ptr<impl::RendererGrid>;

    /// Handle to a generic renderer.
    using Renderer = std::shared_ptr<BaseRenderer>;

    /// Interface class which abstracts different rendering methods.
    /// @details This abstraction allows us to, for example, switch between a
    /// raytracing and a deferred rendering method as we need to, without
    /// changing the API. This is useful since not all computers support
    /// realtime raytracing.
    class BaseRenderer
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        BaseRenderer(core::gl::RenderDevice& renderDevice, glm::uvec2 size);
        BaseRenderer(const BaseRenderer&) = delete;
        virtual ~BaseRenderer() = default;

        /// Uploads a grid to the GPU and returns an handle, which can be used to draw it.
        /// @param grid The grid to upload.
        /// @return The handle of the grid.
        virtual RendererGrid upload(const core::gl::Grid& grid) = 0;

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
                    const core::gl::Framebuffer& target = nullptr);

        /// Gets a reference to the post processing manager.
        /// @return The post processing manager.
        pps::Manager& pps();

    protected:
        core::gl::RenderDevice& mRenderDevice; ///< The render device being used.

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
        /// Called when the internal texture used for post processing needs to be resized.
        void resizeTex(glm::uvec2 size);

        pps::Manager mPpsManager;           ///< The post processing manager.
        core::gl::Framebuffer mFramebuffer; ///< The framebuffer where the frame is drawn.
        core::gl::Texture2D mTexture;       ///< The texture where the frame is drawn.
    };

    /// Abstract types are defined inside this namespace, they should be used (derived) only in renderer
    /// implementations.
    namespace impl
    {
        /// Represents a grid which was uploaded to the GPU.
        class RendererGrid
        {
        public:
            virtual ~RendererGrid() = default;

        protected:
            RendererGrid() = default;
        };
    } // namespace impl
} // namespace cubos::engine::gl
