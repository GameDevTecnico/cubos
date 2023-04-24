#ifndef CUBOS_ENGINE_GL_PPS_COPY_PASS_HPP
#define CUBOS_ENGINE_GL_PPS_COPY_PASS_HPP

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/gl/pps/pass.hpp>

namespace cubos::engine::gl::pps
{
    /// A simple post processing pass that copies the input texture to the output.
    /// This pass is thus useless and is only used as an example and for testing.
    class CopyPass : public Pass
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        CopyPass(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        // Interface methods implementation.

        virtual void resize(glm::uvec2 size) override;
        virtual void execute(std::map<Input, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                             core::gl::Framebuffer out) const override;

    private:
        glm::uvec2 size;                         ///< The size of the window.
        core::gl::ShaderPipeline pipeline;       ///< The shader pipeline to use.
        core::gl::ShaderBindingPoint inputTexBP; ///< The binding point for the input texture.
        core::gl::Sampler inputTexSampler;       ///< The sampler to use for the input texture.
        core::gl::VertexArray screenQuadVA;      ///< The screen quad VA used to render the output.
    };
} // namespace cubos::engine::gl::pps

#endif // CUBOS_ENGINE_GL_PPS_COPY_PASS_HPP
