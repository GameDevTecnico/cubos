#pragma once

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct Num
{
    CUBOS_REFLECT;

    int value;
};

struct Parent
{
    CUBOS_REFLECT;

    cubos::core::ecs::Entity entity;
};
