#ifndef CUBOS_ENGINE_GL_PPS_BLOOM_HPP
#define CUBOS_ENGINE_GL_PPS_BLOOM_HPP

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/gl/pps/pass.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::gl::pps
{
    /// A post processing pass that adds a "bloom" effect to any bright objects
    /// in the scene.
    class BloomPass : public Pass
    {
    public:
        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        BloomPass(core::gl::RenderDevice& renderDevice, glm::uvec2 size);

        /// @param renderDevice The render device to use.
        /// @param size The size of the window.
        /// @param brightnessThreshold The brightness threshold to use.
        /// @param lightBleedStrength The strength of the light bleed effect.
        BloomPass(core::gl::RenderDevice& renderDevice, glm::uvec2 size, float brightnessThreshold, float lightBleedStrength);

        void setBrightnessThreshold(float brightnessThreshold);
        void setLightBleedStrength(float lightBleedStrength);
        float getBrightnessThreshold() const;
        float getLightBleedStrength() const;

        // Interface methods implementation.

        virtual void resize(glm::uvec2 size) override;
        virtual void execute(std::map<Input, core::gl::Texture2D>& inputs, core::gl::Texture2D prev,
                             core::gl::Framebuffer out) const override;

    private:
        float brightnessThreshold; ///< The upper brighness threshold for applying the effect on.
        float lightBleedStrength;  ///< The strength of the light bleed effect

    private:
        glm::uvec2 size;                            ///< The size of the window.
        core::gl::VertexArray screenQuadVA;         ///< The screen quad VA used to render the output.
        core::gl::Sampler texSampler1, texSampler2; ///< The samplers to use for required textures.

    private:
        // Extraction pipeline
        core::gl::Framebuffer extFB;                           ///< The framebuffer of the extraction step.
        core::gl::ShaderPipeline extPipeline;                  ///< The shader pipeline to extract image data required.
        core::gl::ShaderBindingPoint extInputTexBP;            ///< The binding point for the input texture.
        core::gl::ShaderBindingPoint extBrightnessThresholdBP; ///< The binding point for the brightness threshold.
        core::gl::Texture2D extTex;                            ///< The result texture from the extraction step.

    private:
        // Blur pipeline
        core::gl::Framebuffer blurFB;                      ///< The framebuffer to store the blurred bright areas.
        core::gl::ShaderPipeline blurPipeline;             ///< The shader pipeline to blur the bright areas.
        core::gl::ShaderBindingPoint blurInputTexBP;       ///< The binding point for the input texture.
        core::gl::ShaderBindingPoint blurLightBleedBP;     ///< The binding point for the bloom strength.
        core::gl::ShaderBindingPoint blurVerticalBP;       ///< The binding point for knowing when to perform vertical blur.
        core::gl::Texture2D blurTex;                       ///< The result texture from the blur step.

    private:
        // Combine pipeline
        core::gl::ShaderPipeline combinePipeline;        ///< The shader pipeline to combine the results in the final output.
        core::gl::ShaderBindingPoint combineRenderTexBP; ///< The binding point for the initially rendered texture.
        core::gl::ShaderBindingPoint combineBlurTexBP;   ///< The binding point for the blurred result texture.
    };
} // namespace cubos::engine::gl::pps

#endif // CUBOS_ENGINE_GL_PPS_BLOOM_HPP
