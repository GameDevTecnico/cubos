#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    struct Simplex
    {
        // TODO: We should implement something like boost::static_vector for this.
        const std::vector<glm::vec3> points; ///< The points of the simplex.
    };
} // namespace cubos::core::geom