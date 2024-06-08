#pragma once

#include <glm/vec3.hpp>

#include <cubos/engine/prelude.hpp>

struct Obstacle
{
    CUBOS_REFLECT;

    glm::vec3 velocity{0.0F, 0.0F, -1.0F};
    float killZ{0.0F};
};

void obstaclePlugin(cubos::engine::Cubos& cubos);
