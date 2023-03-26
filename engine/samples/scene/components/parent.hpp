#pragma once

#include <cubos/core/ecs/world.hpp>

struct [[cubos::component("parent", VecStorage)]] Parent
{
    cubos::core::ecs::Entity entity;
};
