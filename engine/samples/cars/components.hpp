#pragma once

#include <glm/glm.hpp>

/// [Car component]
struct [[cubos::component("car")]] Car
{
    glm::vec3 vel = {1.0f, 0.0f, 0.0f};
    float angVel = 1.0f;
};
/// [Car component]
