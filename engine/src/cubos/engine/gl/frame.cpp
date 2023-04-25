#include <utility>

#include <cubos/engine/gl/frame.hpp>

using namespace cubos::core::gl;
using namespace cubos::engine::gl;

void Frame::draw(RendererGrid grid, glm::mat4 modelMat)
{
    drawCmds.push_back(DrawCmd{std::move(grid), modelMat});
}

void Frame::ambient(const glm::vec3& color)
{
    this->ambientColor = color;
}

void Frame::light(const SpotLight& light)
{
    spotLights.push_back(light);
}

void Frame::light(const DirectionalLight& light)
{
    directionalLights.push_back(light);
}

void Frame::light(const PointLight& light)
{
    pointLights.push_back(light);
}

void Frame::clear()
{
    drawCmds.clear();
    spotLights.clear();
    directionalLights.clear();
    pointLights.clear();
}

const std::vector<Frame::DrawCmd>& Frame::getDrawCmds() const
{
    return drawCmds;
}

const glm::vec3& Frame::getAmbient() const
{
    return ambientColor;
}

const std::vector<SpotLight>& Frame::getSpotLights() const
{
    return spotLights;
}

const std::vector<DirectionalLight>& Frame::getDirectionalLights() const
{
    return directionalLights;
}

const std::vector<PointLight>& Frame::getPointLights() const
{
    return pointLights;
}
