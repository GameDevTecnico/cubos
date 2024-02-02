#include <cubos/engine/renderer/frame.hpp>

using namespace cubos::engine;

void RendererFrame::draw(RendererGrid grid, glm::mat4 modelMat, uint32_t entityIndex)
{
    mDrawCmds.push_back(DrawCmd{std::move(grid), modelMat, entityIndex});
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

void RendererFrame::light(glm::mat4 transform, const SpotLight& light)
{
    mSpotLights.emplace_back(transform, light);
}

void RendererFrame::light(glm::mat4 transform, const DirectionalLight& light)
{
    mDirectionalLights.emplace_back(transform, light);
}

void RendererFrame::light(glm::mat4 transform, const PointLight& light)
{
    mPointLights.emplace_back(transform, light);
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

const std::vector<std::pair<glm::mat4, SpotLight>>& RendererFrame::spotLights() const
{
    return mSpotLights;
}

const std::vector<std::pair<glm::mat4, DirectionalLight>>& RendererFrame::directionalLights() const
{
    return mDirectionalLights;
}

const std::vector<std::pair<glm::mat4, PointLight>>& RendererFrame::pointLights() const
{
    return mPointLights;
}
