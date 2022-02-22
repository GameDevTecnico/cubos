#ifndef CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
#define CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP

#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

namespace cubos::rendering
{
    class CSMShadowMapper : ShadowMapper
    {
    public:
        CSMShadowMapper();
        void bind() override;
        void unbind() override;
        void clear() override;
        void addLight(const gl::SpotLightData& light) override;
        void addLight(const gl::DirectionalLightData& light) override;
        void addLight(const gl::PointLightData& light) override;
        size_t getSpotOutput(gl::Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices) override;
        size_t getDirectionalOutput(std::vector<gl::Texture2D>& maps, std::vector<glm::mat4>& matrices) override;
        size_t getDirectionalOutput(std::vector<gl::Texture2DArray>& maps, std::vector<glm::mat4>& matrices) override;
    };
} // namespace cubos::rendering

#endif // CUBOS_RENDERING_SHADOW_MAPPING_CSM_SHADOW_MAPPER_HPP
