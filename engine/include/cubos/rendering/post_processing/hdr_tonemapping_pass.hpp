#ifndef CUBOS_RENDERING_POST_PROCESSING_HDR_TONEMAPPING_PASS_HPP
#define CUBOS_RENDERING_POST_PROCESSING_HDR_TONEMAPPING_PASS_HPP

#include <cubos/rendering/post_processing/post_processing_pass.hpp>

#include <cubos/gl/render_device.hpp>

namespace cubos::rendering
{
    class HDRTonemappingPass : public PostProcessingPass
    {
    private:
        gl::ShaderPipeline pipeline;
        gl::ShaderBindingPoint exposureBP, gammaBP;
        gl::ShaderBindingPoint inputTexBP;
        gl::Sampler inputTexSampler;

    public:
        float exposure = 1.0f;
        float gamma = 2.2f;

    public:
        explicit HDRTonemappingPass(io::Window& window);
        HDRTonemappingPass(io::Window& window, float exposure, float gamma);

        virtual void execute(const Renderer& renderer, gl::Texture2D input, gl::Framebuffer output) const override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_POST_PROCESSING_HDR_TONEMAPPING_PASS_HPP
