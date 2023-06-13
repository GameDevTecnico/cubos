#pragma once

#include <cubos/core/geom/plane.hpp>

namespace cubos::engine
{
    struct [[cubos::component("cubos/plane_collider", VecStorage)]] PlaneCollider
    {
        glm::vec3 offset;
        cubos::core::geom::Plane shape;
    };
}