#pragma once

#include <glm/glm.hpp>

struct [[cubos::component("car", VecStorage)]] Car
{
    glm::vec3 vel = {1.0f, 0.0f, 0.0f};
    float angVel = 1.0f;
};
