
#ifndef CUBOS_ENGINE_GL_PPS_COPY_PASS_HPP
#define CUBOS_ENGINE_GL_PPS_COPY_PASS_HPP

#include <cubos/engine/gl/pps/pass.hpp>

namespace cubos::engine::gl::pps
{
    class CopyPass : public PostProcessingPass
    {
    private:
        core::gl::ShaderPipeline pipeline;
        core::gl::ShaderBindingPoint inputTexBP;
        core::gl::Sampler inputTexSampler;

    public:
        explicit CopyPass(core::io::Window& window);

    private:
        void execute(const Renderer& renderer, core::gl::Texture2D input, core::gl::Framebuffer output) const override;
    };
} // namespace cubos::engine::gl::pps

#endif // CUBOS_ENGINE_GL_PPS_COPY_PASS_HPP
