#include <cubos/rendering/shadow_mapping/shadow_mapper.hpp>

#include <glm/gtx/quaternion.hpp>

using namespace cubos;
using namespace cubos::gl;
using namespace cubos::rendering;

ShadowMapper::ShadowMapper(RenderDevice& renderDevice) : renderDevice(renderDevice)
{
}

ShadowMapper::SpotOutput ShadowMapper::getSpotOutput()
{
    return {};
}

ShadowMapper::DirectionalOutput ShadowMapper::getDirectionalOutput()
{
    return {};
}

void ShadowMapper::drawModel(const Renderer::DrawRequest& model)
{
}
