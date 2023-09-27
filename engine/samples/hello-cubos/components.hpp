#pragma once

#include <cubos/core/ecs/world.hpp>

struct [[cubos::component("num", VecStorage)]] Num
{
    int value;
};
