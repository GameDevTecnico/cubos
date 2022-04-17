
#ifndef CUBOS_ENGINE_RENDERING_POST_PROCESSING_COPY_PASS_HPP
#define CUBOS_ENGINE_RENDERING_POST_PROCESSING_COPY_PASS_HPP

#include <cubos/engine/rendering/post_processing/post_processing_pass.hpp>

namespace cubos::engine::rendering
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
} // namespace cubos::engine::rendering

#endif // CUBOS_ENGINE_RENDERING_POST_PROCESSING_COPY_PASS_HPP
