/// @file
/// @brief Struct @ref cubos::core::geom::PolygonalFeature.
/// @ingroup core-geom

#pragma once

#include <vector>

#include <glm/vec3.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a polygonal feature. Used internally for manifold computation.
    /// @ingroup core-geom
    struct PolygonalFeature
    {
        uint32_t faceId;
        std::vector<uint32_t> vertexIds;
        std::vector<glm::vec3> vertices;
        glm::vec3 normal;
    };
} // namespace cubos::core::geom
