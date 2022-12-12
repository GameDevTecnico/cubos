#ifndef COMPONENTS_PARENT_HPP
#define COMPONENTS_PARENT_HPP

#include <components/base.hpp>

#include <cubos/core/ecs/world.hpp>

struct [[cubos::component("parent", VecStorage)]] Parent
{
    cubos::core::ecs::Entity entity;
};

#endif // COMPONENTS_PARENT_HPP
