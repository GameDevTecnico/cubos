#pragma once

#include <map>

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/renderer/pps/manager.hpp>

namespace cubos::engine
{
    /// A generic post processing pass.
    /// All passes must have the same constructor arguments: the render device and the size of the window.
    /// They should never be constructed directly, and instead should be created using the PostProcessingManager.
    ///
    /// @see PostProcessingManager
    class PostProcessingPass
    {
    public:
        /// @param renderDevice The render device to use.
        PostProcessingPass(core::gl::RenderDevice& renderDevice);
        PostProcessingPass(const PostProcessingPass&) = delete;
        virtual ~PostProcessingPass() = default;

        /// Called when the window framebuffer size changes.
        /// @param size The new size of the window.
        virtual void resize(glm::uvec2 size) = 0;

        /// Called each frame.
        /// @param inputs The available extra input textures. The reference is mutable so that a pass can provide its
        /// own inputs.
        /// @param prev The resulting texture of the previous pass.
        /// @param out The framebuffer where the pass will render to.
        virtual void execute(std::map<PostProcessingInput, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                             core::gl::Framebuffer out) const = 0;

    protected:
        core::gl::RenderDevice& mRenderDevice; ///< The render device to use.
    };
} // namespace cubos::engine
