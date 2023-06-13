#pragma once

#include <cubos/core/geom/capsule.hpp>

namespace cubos::engine
{
    struct [[cubos::component("cubos/capsule_collider", VecStorage)]] CapsuleCollider
    {
        glm::mat4 transform;
        cubos::core::geom::Capsule shape;
    };
}