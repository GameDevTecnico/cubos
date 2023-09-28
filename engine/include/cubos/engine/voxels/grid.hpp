/// @file
/// @brief Class @ref cubos::engine::VoxelGrid.
/// @ingroup voxels-plugin

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>

namespace cubos::engine
{
    class VoxelPalette;
    class VoxelGrid;
} // namespace cubos::engine

namespace cubos::core::data::old
{
    void serialize(Serializer& serializer, const engine::VoxelGrid& grid, const char* name);
    void deserialize(Deserializer& deserializer, engine::VoxelGrid& grid);
} // namespace cubos::core::data::old

namespace cubos::engine
{
    /// @brief Represents a voxel object using a 3D grid.
    /// @see Each voxel stores a material index to be used with a @ref VoxelPalette.
    /// @ingroup voxels-plugin
    class VoxelGrid final
    {
    public:
        ~VoxelGrid() = default;

        /// @brief Constructs an empty single-voxel grid.
        VoxelGrid();

        /// @brief Constructs an empty grid with the given size.
        /// @param size Size of the grid.
        VoxelGrid(const glm::uvec3& size);

        /// @brief Constructs a grid with the given size and initial data.
        /// @note The voxel data is stored in a flat array. The index of a voxel at position `(x, y, z)` is
        /// `x + y * size.x + z * size.x * size.y`.
        /// @param size Size of the grid.
        /// @param indices Material indices of the voxels.
        VoxelGrid(const glm::uvec3& size, const std::vector<uint16_t>& indices);

        /// @brief Move constructs.
        /// @param other Other grid.
        VoxelGrid(VoxelGrid&& other) noexcept;

        /// @brief Makes this grid a copy of another grid.
        /// @param rhs Other grid.
        /// @return This grid, for chaining.
        VoxelGrid& operator=(const VoxelGrid& rhs);

        /// @brief Resizes the grid. New voxels are initialized to 0.
        /// @param size New size of the grid.
        void setSize(const glm::uvec3& size);

        /// @brief Gets the size of the grid.
        /// @return Size of the grid.
        const glm::uvec3& size() const;

        /// @brief Sets all voxels to 0.
        void clear();

        /// @brief Sets the material index of a voxel.
        /// @param position Voxel coordinates.
        /// @param mat Material index to set.
        void set(const glm::ivec3& position, uint16_t mat);

        /// @brief Gets the material index of a voxel.
        /// @param position Voxel coordinates.
        /// @return Material index of the voxel.
        uint16_t get(const glm::ivec3& position) const;

        /// @brief Converts the material indices of this grid from one palette to another.
        ///
        /// For each material, it will search for another material in the second palette which is
        /// similar enough to the original one. The conversion fails if no matching index is found.
        ///
        /// @param src Original palette.
        /// @param dst New palette.
        /// @param minSimilarity Minimum similarity between two materials to consider them the same.
        /// @return Whether the conversion was successful.
        bool convert(const VoxelPalette& src, const VoxelPalette& dst, float minSimilarity);

    private:
        friend void core::data::old::serialize(core::data::old::Serializer& /*serializer*/, const VoxelGrid& /*grid*/,
                                               const char* /*name*/);
        friend void core::data::old::deserialize(core::data::old::Deserializer& /*deserializer*/, VoxelGrid& /*grid*/);

        glm::uvec3 mSize;               ///< Size of the grid.
        std::vector<uint16_t> mIndices; ///< Indices of the grid.
    };
} // namespace cubos::engine
