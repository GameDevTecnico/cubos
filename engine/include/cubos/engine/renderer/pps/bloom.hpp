/// @file
/// @brief Post processing pass implementation @ref cubos::engine::PostProcessingBloom.
/// @ingroup renderer-plugin

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/renderer/pps/pass.hpp>

namespace cubos::engine
{
    /// @brief A post processing pass that adds a "bloom" effect to any bright objects in the scene.
    ///
    /// Implementation based on the following
    /// [tutorial](https://catlikecoding.com/unity/tutorials/advanced-rendering/bloom).
    ///
    /// @ingroup renderer-plugin
    class CUBOS_ENGINE_API PostProcessingBloom : public PostProcessingPass
    {
    public:
        /// @brief Constructs with default arguments.
        /// @param renderDevice Render device to use.
        /// @param size Size of the window.
        PostProcessingBloom(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        /// @brief Constructs with custom settings.
        /// @param renderDevice Render device to use.
        /// @param size Size of the window.
        /// @param iterations Number of iterations for downsample/upsample step
        /// @param threshold Pixel brightness threshold to be considered for bloom effect
        /// @param softThreshold Ratio for including pixels that don't pass the threshold test
        /// @param intensity Intensity of the bloom effect.
        PostProcessingBloom(core::gl::RenderDevice& renderDevice, glm::uvec2 size, unsigned int iterations,
                            float threshold, float softThreshold, float intensity);

        /// @brief Gets the threshold for the bloom effect.
        /// @return Threshold for the bloom effect.
        float getThreshold() const;

        /// @brief Gets the soft threshold ratio.
        /// @return Soft threshold ratio.
        float getSoftThreshold() const;

        /// @brief Gets the intensity of the bloom effect.
        /// @return Intensity of the bloom effect.
        float getIntensity() const;

        /// @brief Sets the threshold for the bloom effect.
        /// @param threshold New threshold.
        void setThreshold(float threshold);

        /// @brief Sets the soft threshold ratio.
        /// @param softThreshold Ratio for including pixels that don't pass the threshold test.
        void setSoftThreshold(float softThreshold);

        /// @brief Sets the intensity of the bloom effect.
        /// @param intensity Intensity of the bloom effect.
        void setIntensity(float intensity);

        /// @brief Generates the textures used by the pass.
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
        std::vector<core::gl::Texture2D> mBloomTexBuffer; ///< Texture buffer of the bloom effect.
        std::vector<core::gl::Framebuffer> mBloomFBs;     ///< Framebuffers of the bloom effect.
    };
} // namespace cubos::engine
