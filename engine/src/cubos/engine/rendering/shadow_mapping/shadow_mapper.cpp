#include <cubos/engine/rendering/shadow_mapping/shadow_mapper.hpp>

#include "glm/gtx/quaternion.hpp"

using namespace cubos::core;
using namespace cubos::core::gl;
using namespace cubos::engine;
using namespace cubos::engine::rendering;

ShadowMapper::ShadowMapper(RenderDevice& renderDevice) : renderDevice(renderDevice)
{
}

ShadowMapper::SpotOutput ShadowMapper::getSpotOutput() const
{
    return {};
}

ShadowMapper::DirectionalOutput ShadowMapper::getDirectionalOutput() const
{
    return {};
}

ShadowMapper::PointOutput ShadowMapper::getPointOutput() const
{
    return {};
}

void ShadowMapper::drawModel(const Renderer::DrawRequest& model)
{
}
