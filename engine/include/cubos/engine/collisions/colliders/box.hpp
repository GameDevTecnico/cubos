#pragma once

#include <cubos/core/geom/box.hpp>

namespace cubos::engine
{
    struct [[cubos::component("cubos/box_collider", VecStorage)]] BoxCollider
    {
        glm::mat4 transform;
        cubos::core::geom::Box shape;
        float margin;
    };
}