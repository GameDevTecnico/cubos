#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/renderer/pps/pass.hpp>

namespace cubos::engine
{
    /// A post processing pass that adds a "bloom" effect to any bright objects
    /// in the scene.
    ///
    /// Source: https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom
    class PostProcessingBloom : public PostProcessingPass
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        PostProcessingBloom(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        /// @param iterations Number of iterations for downsample/upsample step
        /// @param threshold Pixel brightness threshold to be considered for bloom effect
        /// @param softThreshold Ratio for including pixels that don't pass the threshold test
        /// @param intensity The intensity of the bloom effect.
        PostProcessingBloom(core::gl::RenderDevice& renderDevice, glm::uvec2 size, unsigned int iterations,
                            float threshold, float softThreshold, float intensity);

        /// Gets the threshold for the bloom effect.
        /// @return The threshold for the bloom effect.
        float getThreshold() const;

        /// Gets the soft threshold ratio.
        /// @return The soft threshold ratio.
        float getSoftThreshold() const;

        /// Gets the intensity of the bloom effect.
        /// @return The intensity of the bloom effect.
        float getIntensity() const;

        /// Sets the threshold for the bloom effect.
        /// @param threshold The new threshold.
        void setThreshold(float threshold);

        /// Sets the soft threshold ratio.
        /// @param softThreshold Ratio for including pixels that don't pass the threshold test.
        void setSoftThreshold(float softThreshold);

        /// Sets the intensity of the bloom effect.
        /// @param intensity The intensity of the bloom effect.
        void setIntensity(float intensity);

        /// Generates the textures used by the pass.
        void generateTextures();

        // Interface methods implementation.

        void resize(glm::uvec2 size) override;
        void execute(std::map<PostProcessingInput, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                     core::gl::Framebuffer out) const override;

    private:
        unsigned int mIterations; ///< Number of iterations for downscale/upscale operation
        float mThreshold;         ///< Brighness threshold for applying the effect on.
        float mSoftThreshold;     ///< Soft brightness threshold to include some samples under the upper threshold.
        float mIntensity;         ///< Effect intensity

        glm::uvec2 mSize;                    ///< Size of the window.
        core::gl::VertexArray mScreenQuadVa; ///< Screen quad VA used to render the output.
        core::gl::Sampler mTexSampler;       ///< Sampler to use for required textures.
        core::gl::BlendState mBlendState;    ///< Blend state required for upscaling process.

        // Extraction pipeline
        core::gl::Framebuffer mExtFb;                       ///< Framebuffer of the extraction step.
        core::gl::ShaderPipeline mExtPipeline;              ///< Shader pipeline of the extraction step.
        core::gl::ShaderBindingPoint mExtInputTexBp;        ///< Input texture binding point.
        core::gl::ShaderBindingPoint mExtThresholdFilterBp; ///< Threshold information binding point.
        core::gl::Texture2D mExtTex;                        ///< Result texture from extraction step

        // Bloom pipeline
        core::gl::ShaderPipeline mBloomPipeline;          ///< Shader pipeline of the bloom effect.
        core::gl::ShaderBindingPoint mBloomInputTexBp;    ///< Input texture binding point.
        core::gl::ShaderBindingPoint mBloomSrcTexBp;      ///< Source texture binding point.
        core::gl::ShaderBindingPoint mBloomScalingBp;     ///< Texture scaling binding point.
        core::gl::ShaderBindingPoint mBloomCurrPassBp;    ///< Current pass binding point.
        core::gl::ShaderBindingPoint mBloomIntensityBp;   ///< Bloom intensity binding point.
        std::vector<core::gl::Texture2D> mBloomTexBuffer; ///< The texture buffer of the bloom effect.
        std::vector<core::gl::Framebuffer> mBloomFBs;     ///< The framebuffers of the bloom effect.
    };
} // namespace cubos::engine
