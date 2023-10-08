#pragma once

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct [[cubos::component("num", VecStorage)]] Num
{
    CUBOS_REFLECT;

    int value;
};
