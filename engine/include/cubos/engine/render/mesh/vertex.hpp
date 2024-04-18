/// @file
/// @brief Struct @ref cubos::engine::RenderMeshVeretx.
/// @ingroup render-mesh-plugin

#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    class VoxelGrid;

    /// @brief Vertex data type stored in the @ref RenderMeshPool's vertex buffer.
    /// @ingroup render-mesh-plugin
    struct CUBOS_ENGINE_API RenderMeshVertex
    {
        /// @brief Vertex position within the mesh.
        glm::u8vec3 position;

        /// @brief Vertex normal (0 for +X, 1 for -X, 2 for +Y, 3 for -Y, 4 for +Z, 5 for -Z).
        glm::uint8 normal;

        /// @brief Vertex material.
        glm::uint32 material;

        /// @brief Generates a vertex element description for this vertex type.
        ///
        /// Assumes a vertex buffer stores this and only this type, as an array of ivec2, where the x component is the
        /// positionAndSize, and the colorAndNormal is the y component.
        ///
        /// @param position Position attribute name in the pipeline.
        /// @param normal Normal attribute name in the pipeline.
        /// @param material Material attribute name in the pipeline.
        /// @param bufferIndex Index of the vertex buffer where the elements will be stored.
        /// @param desc Vertex array description to add the elements to.
        /// @return Whether the vertex elements fitted in the vertex array description.
        static bool addVertexElements(const char* position, const char* normal, const char* material,
                                      std::size_t bufferIndex, core::gl::VertexArrayDesc& desc);

        /// @brief Generates a vertex mesh for the given voxel grid.
        /// @param grid Voxel grid.
        /// @param[out] vertices Vertices generated from the voxel grid.
        static void generate(const VoxelGrid& grid, std::vector<RenderMeshVertex>& facverticeses);
    };
} // namespace cubos::engine
