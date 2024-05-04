#include "utils.hpp"

#include <glm/gtx/norm.hpp>

using namespace cubos::engine;

float cubos::engine::getLagrangeMultiplierUpdate(float c, const float inverseMasses[], glm::vec3 gradients[],
                                                 int numParticles, float compliance, float dt)
{
    float wGradientSum = 0.0F;
    for (int i = 0; i < numParticles; i++)
    {
        wGradientSum += inverseMasses[i] * glm::length2(gradients[i]);
    }

    float tildeAlpha = compliance / (dt * dt);

    if (wGradientSum + tildeAlpha == 0.0F)
    {
        return 0.0F;
    }

    return (-c) / (wGradientSum + tildeAlpha);
}