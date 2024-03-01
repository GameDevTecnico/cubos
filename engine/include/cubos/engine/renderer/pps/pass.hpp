/// @file
/// @brief Class @ref cubos::engine::PostProcessingPass.
/// @ingroup renderer-plugin

#pragma once

#include <map>

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/renderer/pps/manager.hpp>

namespace cubos::engine
{
    /// @brief A generic post processing pass.
    ///
    /// All passes must have the same constructor arguments: the render device and the size of the
    /// window. They should never be constructed directly, and instead should be created using the
    /// PostProcessingManager.
    ///
    /// @see PostProcessingManager
    /// @ingroup renderer-plugin
    class CUBOS_ENGINE_API PostProcessingPass
    {
    public:
        virtual ~PostProcessingPass() = default;

        /// @brief Constructs.
        /// @param renderDevice Render device to use.
        PostProcessingPass(core::gl::RenderDevice& renderDevice);

        /// @name Forbid any kind of copying.
        /// @{
        PostProcessingPass(const PostProcessingPass&) = delete;
        PostProcessingPass& operator=(const PostProcessingPass&) = delete;
        /// @}

        /// @brief Called when the window framebuffer size changes.
        /// @param size New size of the window.
        virtual void resize(glm::uvec2 size) = 0;

        /// @brief Called each frame.
        ///
        /// The inputs argument is mutable to allow passes to their own inputs for future passes.
        /// This argument is used, for example, to pass the extra outputs of the deferred renderer
        /// to the post processing passes which might need them.
        ///
        /// @param inputs Available extra input textures.
        /// @param prev Resulting texture of the previous pass.
        /// @param out Framebuffer where the pass will render to.
        virtual void execute(std::map<PostProcessingInput, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                             core::gl::Framebuffer out) const = 0;

    protected:
        core::gl::RenderDevice& mRenderDevice; ///< Render device to use.
    };
} // namespace cubos::engine
