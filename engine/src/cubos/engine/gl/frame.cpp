#include <utility>

#include <cubos/engine/gl/frame.hpp>

using namespace cubos::core::gl;
using namespace cubos::engine::gl;

void Frame::draw(RendererGrid grid, glm::mat4 modelMat)
{
    mDrawCmds.push_back(DrawCmd{std::move(grid), modelMat});
}

void Frame::ambient(const glm::vec3& color)
{
    mAmbientColor = color;
}

void Frame::light(const SpotLight& light)
{
    mSpotLights.push_back(light);
}

void Frame::light(const DirectionalLight& light)
{
    mDirectionalLights.push_back(light);
}

void Frame::light(const PointLight& light)
{
    mPointLights.push_back(light);
}

void Frame::clear()
{
    mDrawCmds.clear();
    mSpotLights.clear();
    mDirectionalLights.clear();
    mPointLights.clear();
}

const std::vector<Frame::DrawCmd>& Frame::getDrawCmds() const
{
    return mDrawCmds;
}

const glm::vec3& Frame::getAmbient() const
{
    return mAmbientColor;
}

const std::vector<SpotLight>& Frame::getSpotLights() const
{
    return mSpotLights;
}

const std::vector<DirectionalLight>& Frame::getDirectionalLights() const
{
    return mDirectionalLights;
}

const std::vector<PointLight>& Frame::getPointLights() const
{
    return mPointLights;
}
