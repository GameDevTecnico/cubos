#pragma once

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct Num
{
    CUBOS_REFLECT;

    int value;
};
