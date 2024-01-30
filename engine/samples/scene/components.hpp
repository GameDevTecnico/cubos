#pragma once

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct Num
{
    CUBOS_REFLECT;

    int value;
};

struct OwnedBy
{
    CUBOS_REFLECT;
};

struct DistanceTo
{
    CUBOS_REFLECT;

    int value;
};
