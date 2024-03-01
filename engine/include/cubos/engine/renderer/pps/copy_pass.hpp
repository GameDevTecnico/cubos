/// @file
/// @brief Post processing pass implementation @ref cubos::engine::PostProcessingCopy.
/// @ingroup renderer-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/renderer/pps/pass.hpp>

namespace cubos::engine
{
    /// @brief A simple post processing pass that copies the input texture to the output.
    ///
    /// This pass is useless and is only used as an example and for testing.
    ///
    /// @ingroup renderer-plugin
    class CUBOS_ENGINE_API PostProcessingCopy : public PostProcessingPass
    {
    public:
        /// @brief Constructs.
        /// @param renderDevice Render device to use.
        /// @param size Size of the window.
        PostProcessingCopy(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        // Interface methods implementation.

        void resize(glm::uvec2 size) override;
        void execute(std::map<PostProcessingInput, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                     core::gl::Framebuffer out) const override;

    private:
        glm::uvec2 mSize;                         ///< Size of the window.
        core::gl::ShaderPipeline mPipeline;       ///< Shader pipeline to use.
        core::gl::ShaderBindingPoint mInputTexBp; ///< Binding point for the input texture.
        core::gl::Sampler mInputTexSampler;       ///< Sampler to use for the input texture.
        core::gl::VertexArray mScreenQuadVa;      ///< Screen quad VA used to render the output.
    };
} // namespace cubos::engine
