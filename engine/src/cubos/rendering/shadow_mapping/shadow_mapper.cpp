#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

#include <glm/gtx/quaternion.hpp>

using namespace cubos;
using namespace cubos::gl;
using namespace cubos::rendering;

ShadowMapper::ShadowMapper(RenderDevice& renderDevice) : renderDevice(renderDevice)
{
}

size_t ShadowMapper::getSpotOutput(Texture2DArray& mapAtlas, std::vector<glm::mat4>& matrices)
{
    mapAtlas = nullptr;
    matrices = {};
    return 0;
}

ShadowMapper::DirectionalOutput ShadowMapper::getDirectionalOutput()
{
    return {};
}

void ShadowMapper::drawModel(const Renderer::DrawRequest& model)
{
}
