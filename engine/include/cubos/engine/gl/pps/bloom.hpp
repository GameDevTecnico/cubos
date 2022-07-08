#ifndef CUBOS_ENGINE_GL_PPS_BLOOM_HPP
#define CUBOS_ENGINE_GL_PPS_BLOOM_HPP

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/gl/pps/pass.hpp>

#include <glm/glm.hpp>

#include <vector>

namespace cubos::engine::gl::pps
{
    /// A post processing pass that adds a "bloom" effect to any bright objects
    /// in the scene.
    ///
    /// Source: https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom
    class BloomPass : public Pass
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        BloomPass(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        /// @param iterations Number of iterations for downsample/upsample step
        /// @param threshold Pixel brightness threshold to be considered for bloom effect
        /// @param softThreshold Ratio for including pixels that don't pass the threshold test
        /// @param intensity The intensity of the bloom effect.
        BloomPass(core::gl::RenderDevice& renderDevice, glm::uvec2 size, unsigned int iterations, float threshold, float softThreshold, float intensity);

        float getThreshold() const;
        float getSoftThreshold() const;
        float getIntensity() const;
        void setThreshold(float threshold);
        void setSoftThreshold(float softThreshold);
        void setIntensity(float intensity);

        void generateTextures();

        // Interface methods implementation.
        virtual void resize(glm::uvec2 size) override;
        virtual void execute(std::map<Input, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                             core::gl::Framebuffer out) const override;

    private:
        unsigned int iterations; ///< Number of iterations for downscale/upscale operation
        float threshold;         ///< Brighness threshold for applying the effect on.
        float softThreshold;     ///< Soft brightness threshold to include some samples under the upper threshold.
        float intensity;         ///< Effect intensity

    private:
        glm::uvec2 size;                    ///< Size of the window.
        core::gl::VertexArray screenQuadVA; ///< Screen quad VA used to render the output.
        core::gl::Sampler texSampler;       ///< Sampler to use for required textures.
        core::gl::BlendState blendState;    ///< Blend state required for upscaling process.

    private:
        // Extraction pipeline
        core::gl::Framebuffer extFB;                       ///< Framebuffer of the extraction step.
        core::gl::ShaderPipeline extPipeline;              ///< Shader pipeline of the extraction step.
        core::gl::ShaderBindingPoint extInputTexBP;        ///< Input texture binding point.
        core::gl::ShaderBindingPoint extThresholdFilterBP; ///< Threshold information binding point.
        core::gl::Texture2D extTex;                        ///< Result texture from extraction step

    private:
        // Bloom pipeline
        core::gl::ShaderPipeline bloomPipeline;          ///< Shader pipeline of the bloom effect.
        core::gl::ShaderBindingPoint bloomInputTexBP;    ///< Input texture binding point.
        core::gl::ShaderBindingPoint bloomSrcTexBP;      ///< Source texture binding point.
        core::gl::ShaderBindingPoint bloomScalingBP;     ///< Texture scaling binding point.
        core::gl::ShaderBindingPoint bloomCurrPassBP;    ///< Current pass binding point.
        core::gl::ShaderBindingPoint bloomIntensityBP;   ///< Bloom intensity binding point.
        std::vector<core::gl::Texture2D> bloomTexBuffer; ///< The texture buffer of the bloom effect.
        std::vector<core::gl::Framebuffer> bloomFBs;     ///< The framebuffers of the bloom effect.


    };
} // namespace cubos::engine::gl::pps

#endif // CUBOS_ENGINE_GL_PPS_BLOOM_HPP
