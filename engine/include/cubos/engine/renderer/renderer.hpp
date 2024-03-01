/// @file
/// @brief Class @ref cubos::engine::BaseRenderer and resource @ref cubos::engine::Renderer.
/// @ingroup renderer-plugin

/// @dir ./pps
/// @brief Post processing directory.

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/renderer/camera.hpp>
#include <cubos/engine/renderer/pps/manager.hpp>
#include <cubos/engine/renderer/vertex.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/palette.hpp>

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
        /// @brief Struct which holds the viewport information for a camera, to be used for drawing.
        struct Viewport
        {
            glm::ivec2 position;
            glm::ivec2 size;
        };

        virtual ~BaseRenderer() = default;

        /// @brief Constructs.
        /// @warning @p renderDevice must be valid during the lifetime of the renderer.
        /// @param renderDevice Render device to use.
        /// @param size Size of the window.
        BaseRenderer(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        /// @brief Deleted copy constructor.
        BaseRenderer(const BaseRenderer&) = delete;

        /// @brief Uploads a grid to the GPU and returns an handle which can be used to draw it.
        /// @param grid Grid to upload.
        /// @return Handle of the grid.
        virtual RendererGrid upload(const VoxelGrid& grid) = 0;

        /// @brief Sets the current palette of the renderer.
        /// @param palette Palette to set.
        virtual void setPalette(const VoxelPalette& palette) = 0;

        /// @brief Resizes the renderer's framebuffers.
        /// @param size New size of the window.
        void resize(glm::uvec2 size);

        /// @brief Gets the current size of the renderer's framebuffers.
        /// @return Current size.
        glm::uvec2 size() const;

        /// @brief Draws a frame.
        /// @param view Camera view transform.
        /// @param viewport Camera viewport.
        /// @param camera Camera to use.
        /// @param frame Frame to draw.
        /// @param pickingBuffer Screen picking framebuffer.
        /// @param usePostProcessing Whether to use post processing.
        /// @param target Target framebuffer to draw to.
        void render(const glm::mat4& view, const Viewport& viewport, const engine::Camera& camera,
                    const RendererFrame& frame, const core::gl::Framebuffer& pickingBuffer = nullptr,
                    bool usePostProcessing = true, const core::gl::Framebuffer& target = nullptr);

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
        /// @param view Camera view transform.
        /// @param viewport Camera viewport.
        /// @param camera Camera to use.
        /// @param frame Frame to draw.
        /// @param target Target framebuffer.
        /// @param pickingBuffer Screen picking framebuffer.
        virtual void onRender(const glm::mat4& view, const Viewport& viewport, const Camera& camera,
                              const RendererFrame& frame, core::gl::Framebuffer target,
                              core::gl::Framebuffer pickingBuffer) = 0;

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

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, cubos::engine::Renderer);
