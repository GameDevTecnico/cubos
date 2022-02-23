#ifndef CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
#define CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP

#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

#define CUBOS_CSM_MAX_SPOT_SHADOW_COUNT 32
#define CUBOS_CSM_MAX_DIRECTIONAL_SHADOW_COUNT 8
#define CUBOS_CSM_MAX_POINT_SHADOW_COUNT 8

#define CUBOS_CSM_SHADOW_CASCADE_COUNT 5

namespace cubos::rendering
{
    class CSMShadowMapper : public ShadowMapper
    {
    private:
        gl::Texture2DArray spotAtlas;
        gl::Texture2DArray directionalAtlas;

        gl::Framebuffer spotFramebuffer;
        gl::Framebuffer directionalFramebuffer;

        size_t resolution;

        gl::ShaderPipeline spotPipeline;
        gl::ShaderPipeline directionalPipeline;

    private:
        inline void setupFramebuffers();
        inline void setupPipelines();

    public:
        CSMShadowMapper(gl::RenderDevice& renderDevice, size_t resolution);

        virtual void setModelMatrix(glm::mat4 modelMat) override;
        virtual void bind() override;
        virtual void unbind() override;
        virtual void clear() override;
        virtual void addLight(const gl::SpotLightData& light) override;
        virtual void addLight(const gl::DirectionalLightData& light) override;
        virtual void addLight(const gl::PointLightData& light) override;
        virtual size_t getSpotOutput(gl::Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices) override;
        virtual size_t getDirectionalOutput(gl::Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices,
                                            size_t& atlasStride) override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
