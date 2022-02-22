#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

#include <glm/gtx/quaternion.hpp>

cubos::rendering::ShadowMapper::SpotLightData::SpotLightData(
    const cubos::gl::SpotLightData& light)
    : position(glm::vec4(light.position, 1)), rotation(glm::toMat4(light.rotation)), range(light.range),
      spotAngle(cos(light.spotAngle))
{
}

cubos::rendering::ShadowMapper::DirectionalLightData::DirectionalLightData(const cubos::gl::DirectionalLightData& light)
    : rotation(glm::toMat4(light.rotation))
{
}

cubos::rendering::ShadowMapper::PointLightData::PointLightData(
    const cubos::gl::PointLightData& light)
    : position(glm::vec4(light.position, 1)), range(light.range)
{
}

size_t cubos::rendering::ShadowMapper::getSpotOutput(cubos::gl::Texture2DArray& mapAtlas,
                                                     std::vector<glm::mat4>& matrices)
{
    mapAtlas = nullptr;
    return 0;
}

size_t cubos::rendering::ShadowMapper::getDirectionalOutput(std::vector<gl::Texture2D>& maps,
                                                            std::vector<glm::mat4>& matrices)
{
    maps = {};
    matrices = {};
    return 0;
}

size_t cubos::rendering::ShadowMapper::getDirectionalOutput(std::vector<gl::Texture2DArray>& maps,
                                                            std::vector<glm::mat4>& matrices)
{
    maps = {};
    matrices = {};
    return 0;
}
