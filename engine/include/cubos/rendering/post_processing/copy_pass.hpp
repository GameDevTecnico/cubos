
#ifndef CUBOS_RENDERING_POST_PROCESSING_COPY_PASS_HPP
#define CUBOS_RENDERING_POST_PROCESSING_COPY_PASS_HPP

#include <cubos/rendering/post_processing/post_processing_pass.hpp>

namespace cubos::rendering
{
    class CopyPass : PostProcessingPass
    {
    private:
        gl::ShaderPipeline pipeline;
        gl::ShaderBindingPoint inputTexBP;
        gl::Sampler inputTexSampler;

        // Screen Quad
        gl::VertexArray screenVertexArray;
        gl::IndexBuffer screenIndexBuffer;

    public:
        explicit CopyPass(io::Window& window);

    private:
        void execute(const Renderer& renderer, gl::Texture2D input, gl::Framebuffer output) const override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_POST_PROCESSING_COPY_PASS_HPP
