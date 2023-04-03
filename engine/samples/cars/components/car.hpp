#ifndef COMPONENTS_CAR_HPP
#define COMPONENTS_CAR_HPP

#include <components/base.hpp>

#include <glm/glm.hpp>

struct [[cubos::component("car", VecStorage)]] Car
{
    glm::vec3 vel = {1.0f, 0.0f, 0.0f};
    float angVel = 1.0f;
};

#endif // COMPONENTS_CAR_HPP