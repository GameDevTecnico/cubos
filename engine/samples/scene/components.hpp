#pragma once

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct [[cubos::component("num", VecStorage)]] Num
{
    CUBOS_REFLECT;

    int value;
};

struct [[cubos::component("parent", VecStorage)]] Parent
{
    CUBOS_REFLECT;

    cubos::core::ecs::Entity entity;
};
