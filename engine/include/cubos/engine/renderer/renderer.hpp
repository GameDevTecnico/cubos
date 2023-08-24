/// @file
/// @brief Class @ref cubos::engine::BaseRenderer and resource @ref cubos::engine::Renderer.
/// @ingroup renderer-plugin

/// @dir ./pps
/// @brief Post processing directory.

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/camera.hpp>
#include <cubos/core/gl/grid.hpp>
#include <cubos/core/gl/light.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/vertex.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/renderer/pps/manager.hpp>

namespace cubos::engine
{
    class RendererFrame;
    class BaseRenderer;
    class PostProcessingPass;

    namespace impl
    {
        class RendererGrid;
    }

    /// @brief Handle to a grid uploaded to the GPU, to be used for rendering.
    /// @ingroup renderer-plugin
    using RendererGrid = std::shared_ptr<impl::RendererGrid>;

    /// @brief Resource which is an handle to a generic renderer.
    /// @ingroup renderer-plugin
    using Renderer = std::shared_ptr<BaseRenderer>;

    /// @brief Interface which abstracts different rendering methods.
    ///
    /// This abstraction allows us to, for example, switch between a raytracing and a deferred
    /// rendering method as we need to, without changing the API. This is useful since not all
    /// computers support realtime raytracing.
    ///
    /// @note This code was written previously to the development of the @ref Cubos class, and
    /// thats why it is structured this way. In the future it would be better to simply abstract
    /// away renderer implementations using different plugins which would just switch the systems
    /// being run, keeping the same components which are effectively its API.
    ///
    /// @see PostProcessingManager
    /// @ingroup renderer-plugin
    class BaseRenderer
    {
    public:
        virtual ~BaseRenderer() = default;

        /// @brief Constructs.
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        BaseRenderer(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        /// @brief Deleted copy constructor.
        BaseRenderer(const BaseRenderer&) = delete;

        /// @brief Uploads a grid to the GPU and returns an handle which can be used to draw it.
        /// @param grid Grid to upload.
        /// @return Handle of the grid.
        virtual RendererGrid upload(const core::gl::Grid& grid) = 0;

        /// @brief Sets the current palette of the renderer.
        /// @param palette Palette to set.
        virtual void setPalette(const core::gl::Palette& palette) = 0;

        /// @brief Resizes the renderer's framebuffers.
        /// @param size New size of the window.
        void resize(glm::uvec2 size);

        /// @brief Gets the current size of the renderer's framebuffers.
        /// @return Current size.
        glm::uvec2 size() const;

        /// @brief Draws a frame.
        /// @param camera Camera to use.
        /// @param frame Frame to draw.
        /// @param usePostProcessing Whether to use post processing.
        /// @param target Target framebuffer to draw to.
        void render(const core::gl::Camera& camera, const RendererFrame& frame, bool usePostProcessing = true,
                    const core::gl::Framebuffer& target = nullptr);

        /// @brief Gets a reference to the post processing manager.
        /// @return Post processing manager.
        PostProcessingManager& pps();

    protected:
        core::gl::RenderDevice& mRenderDevice; ///< Render device being used.

        /// @brief Called when resize() is called.
        ///
        /// Renderer implementations should override this function to resize their framebuffers.
        ///
        /// @param size New size of the framebuffer.
        virtual void onResize(glm::uvec2 size) = 0;

        /// @brief Called when render() is called, before applying post processing effects.
        ///
        /// Renderer implementations should implement this function to draw the frame.
        /// When post processing is enabled, the target framebuffer will be the internal texture
        /// which will be used for post processing.
        ///
        /// @param camera Camera to use.
        /// @param frame Frame to draw.
        /// @param target Target framebuffer.
        virtual void onRender(const core::gl::Camera& camera, const RendererFrame& frame,
                              core::gl::Framebuffer target) = 0;

    private:
        /// @brief Called when the internal texture used for post processing needs to be resized.
        void resizeTex(glm::uvec2 size);

        PostProcessingManager mPpsManager;  ///< Post processing manager.
        core::gl::Framebuffer mFramebuffer; ///< Framebuffer where the frame is drawn.
        core::gl::Texture2D mTexture;       ///< Texture where the frame is drawn.
        glm::uvec2 mSize;
    };

    /// @brief Namespace to store the abstract types implemented by the renderer implementations.
    namespace impl
    {
        /// @brief Represents a grid which was uploaded to the GPU.
        class RendererGrid
        {
        public:
            virtual ~RendererGrid() = default;

        protected:
            RendererGrid() = default;
        };
    } // namespace impl
} // namespace cubos::engine
