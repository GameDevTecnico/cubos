#pragma once

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

struct Spawner
{
    CUBOS_REFLECT;

    cubos::engine::Asset<cubos::engine::Scene> scene; // Scene which will be spawned
    float period{1.0F};                               // Time between spawns in seconds
    float laneWidth{1.0F};                            // Width of the lane
    float accumulator{0.0F};                          // Time accumulator
};

void spawnerPlugin(cubos::engine::Cubos& cubos);
