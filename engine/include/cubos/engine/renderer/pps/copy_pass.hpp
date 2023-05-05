#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/renderer/pps/pass.hpp>

namespace cubos::engine
{
    /// A simple post processing pass that copies the input texture to the output.
    /// This pass is thus useless and is only used as an example and for testing.
    class PostProcessingCopy : public PostProcessingPass
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        PostProcessingCopy(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        // Interface methods implementation.

        void resize(glm::uvec2 size) override;
        void execute(std::map<PostProcessingInput, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                     core::gl::Framebuffer out) const override;

    private:
        glm::uvec2 mSize;                         ///< The size of the window.
        core::gl::ShaderPipeline mPipeline;       ///< The shader pipeline to use.
        core::gl::ShaderBindingPoint mInputTexBp; ///< The binding point for the input texture.
        core::gl::Sampler mInputTexSampler;       ///< The sampler to use for the input texture.
        core::gl::VertexArray mScreenQuadVa;      ///< The screen quad VA used to render the output.
    };
} // namespace cubos::engine
