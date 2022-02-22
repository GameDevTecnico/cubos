#include <cubos/rendering/shadow_mapping/csm_shadow_mapper.hpp>

void cubos::rendering::CSMShadowMapper::bind()
{
}

void cubos::rendering::CSMShadowMapper::unbind()
{
}

void cubos::rendering::CSMShadowMapper::clear()
{
}

void cubos::rendering::CSMShadowMapper::addLight(const cubos::gl::SpotLightData& light)
{
}

void cubos::rendering::CSMShadowMapper::addLight(const cubos::gl::DirectionalLightData& light)
{
}

void cubos::rendering::CSMShadowMapper::addLight(const cubos::gl::PointLightData& light)
{
}

size_t cubos::rendering::CSMShadowMapper::getSpotOutput(cubos::gl::Texture2DArray& mapAtlas,
                                                        std::vector<glm::mat4>& matrices)
{
    return ShadowMapper::getSpotOutput(mapAtlas, matrices);
}

size_t cubos::rendering::CSMShadowMapper::getDirectionalOutput(std::vector<gl::Texture2D>& maps,
                                                               std::vector<glm::mat4>& matrices)
{
    return ShadowMapper::getDirectionalOutput(maps, matrices);
}

size_t cubos::rendering::CSMShadowMapper::getDirectionalOutput(std::vector<gl::Texture2DArray>& maps,
                                                               std::vector<glm::mat4>& matrices)
{
    return ShadowMapper::getDirectionalOutput(maps, matrices);
}
