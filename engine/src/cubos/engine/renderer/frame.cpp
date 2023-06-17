#include <utility>

#include <cubos/engine/renderer/frame.hpp>

using cubos::core::gl::DirectionalLight;
using cubos::core::gl::PointLight;
using cubos::core::gl::SpotLight;
using cubos::engine::RendererFrame;
using cubos::engine::RendererGrid;

void RendererFrame::draw(RendererGrid grid, glm::mat4 modelMat)
{
    mDrawCmds.push_back(DrawCmd{std::move(grid), modelMat});
}

void RendererFrame::ambient(const glm::vec3& color)
{
    mAmbientColor = color;
}

void RendererFrame::skyGradient(glm::vec3 bottom, glm::vec3 top)
{
    mSkyGradient[0] = bottom;
    mSkyGradient[1] = top;
}

void RendererFrame::light(const SpotLight& light)
{
    mSpotLights.push_back(light);
}

void RendererFrame::light(const DirectionalLight& light)
{
    mDirectionalLights.push_back(light);
}

void RendererFrame::light(const PointLight& light)
{
    mPointLights.push_back(light);
}

void RendererFrame::clear()
{
    mDrawCmds.clear();
    mSpotLights.clear();
    mDirectionalLights.clear();
    mPointLights.clear();
}

const std::vector<RendererFrame::DrawCmd>& RendererFrame::drawCmds() const
{
    return mDrawCmds;
}

const glm::vec3& RendererFrame::ambient() const
{
    return mAmbientColor;
}

const glm::vec3& RendererFrame::skyGradient(int i) const
{
    return mSkyGradient[i];
}

const std::vector<SpotLight>& RendererFrame::spotLights() const
{
    return mSpotLights;
}

const std::vector<DirectionalLight>& RendererFrame::directionalLights() const
{
    return mDirectionalLights;
}

const std::vector<PointLight>& RendererFrame::pointLights() const
{
    return mPointLights;
}
