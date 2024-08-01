/// @file
/// @brief Class @ref cubos::engine::VoxelModel
/// @ingroup voxels-plugin

#pragma once

#include <vector>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/palette.hpp>

namespace cubos::engine
{
    struct QBMatrix
    {
        VoxelGrid grid;      ///< Grid of the matrix.
        glm::ivec3 position; ///< Position of the matrix.
    };
} // namespace cubos::engine

namespace cubos::engine
{
    class VoxelModel;
    class VoxelGrid;
    class VoxelPalette;
} // namespace cubos::engine

namespace cubos::engine
{
    /// @brief Holds a VoxelPallete & Grids
    ///
    /// @ingroup voxels-plugin
    class CUBOS_ENGINE_API VoxelModel final
    {
    public:
        CUBOS_REFLECT;

        ~VoxelModel() = default;

        /// @brief Constructs an empty VoxelModel
        VoxelModel() = default;

        uint16_t getMatricesSize() const;

        const VoxelPalette& getPalette() const;

        VoxelGrid& getGrid(uint16_t index);

        /// @brief Loads the grid's data from the given stream.
        ///
        /// Assumes the data is stored in big-endian (network byte order).
        /// The first bytes correspond to three uint32_t, which represent the size of the grid (x, y, z).
        /// The next bytes correspond to `size.x * size.y * size.z` uint16_t, which represent the actual voxel
        /// materials. The voxel data is indexed by `x + y * size.x + z * size.x * size.y`.
        ///
        /// @param stream Stream to read from.
        /// @return Whether the stream contained valid data.
        bool loadFrom(core::memory::Stream& stream);

        /// @brief Writes the VoxelModel's data to the given stream.
        ///
        /// Writes in the format specified in @ref loadFrom.
        ///
        /// @param stream Stream to write to.
        /// @return Whether the write was successful.
        bool writeTo(core::memory::Stream& stream) const;

    private:
        std::vector<QBMatrix> matrices;
        VoxelPalette palette;
    };
} // namespace cubos::engine
