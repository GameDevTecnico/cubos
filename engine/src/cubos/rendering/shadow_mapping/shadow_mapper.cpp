#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

#include <glm/gtx/quaternion.hpp>

cubos::rendering::ShadowMapper::ShadowMapper(cubos::gl::RenderDevice& renderDevice) : renderDevice(renderDevice)
{
}

cubos::rendering::ShadowMapper::SpotLightData::SpotLightData(const cubos::gl::SpotLightData& light)
    : position(glm::vec4(light.position, 1)), rotation(glm::toMat4(light.rotation)), range(light.range),
      spotAngle(cos(light.spotAngle))
{
}

cubos::rendering::ShadowMapper::DirectionalLightData::DirectionalLightData(const cubos::gl::DirectionalLightData& light)
    : rotation(glm::toMat4(light.rotation))
{
}

cubos::rendering::ShadowMapper::PointLightData::PointLightData(const cubos::gl::PointLightData& light)
    : position(glm::vec4(light.position, 1)), range(light.range)
{
}
size_t cubos::rendering::ShadowMapper::getSpotOutput(cubos::gl::Texture2DArray& mapAtlas,
                                                     std::vector<glm::mat4>& matrices)
{
    mapAtlas = nullptr;
    matrices = {};
    return 0;
}

size_t cubos::rendering::ShadowMapper::getDirectionalOutput(cubos::gl::Texture2DArray& mapAtlas,
                                                            std::vector<glm::mat4>& matrices, size_t& atlasStride)
{
    mapAtlas = nullptr;
    matrices = {};
    atlasStride = 0;
    return 0;
}
