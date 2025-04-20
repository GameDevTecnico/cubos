/// @file
/// @brief Component @ref cubos::engine::RenderMesh.
/// @ingroup render-mesh-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/geom/box.hpp>
#include <cubos/engine/render/mesh/pool.hpp>

namespace cubos::engine
{
    /// @brief Component used to draw meshes stored in the vertex pool.
    /// @ingroup render-mesh-plugin
    struct CUBOS_ENGINE_API RenderMesh
    {
        CUBOS_REFLECT;

        /// @brief First bucket in the vertex pool which is part of the mesh.
        RenderMeshPool::BucketId firstBucketId{};

        /// @brief Base offset of the mesh.
        glm::vec3 baseOffset{};

        /// @brief Bounding box of the mesh.
        cubos::core::geom::Box boundingBox{};
    };
} // namespace cubos::engine
