#include <cubos/rendering/shadow_mapping/shadow_mapper.h>
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

size_t cubos::rendering::ShadowMapper::getSpotOutput(cubos::gl::Texture2DArray& mapAtlas,
                                                     std::vector<glm::mat4>& matrices)
{
    mapAtlas = nullptr;
    return 0;
}
