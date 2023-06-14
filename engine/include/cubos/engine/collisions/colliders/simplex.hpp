#pragma once

#include <cubos/core/geom/simplex.hpp>

namespace cubos::engine
{
    struct [[cubos::component("cubos/simplex", VecStorage)]] SimplexCollider
    {
        glm::vec3 offset;
        cubos::core::geom::Simplex shape;
        float margin;
    };
}
