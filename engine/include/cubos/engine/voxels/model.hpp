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
    /// @brief Holds the data loaded from voxel model files, such as Qubicle models.
    ///
    /// @ingroup voxels-plugin
    class CUBOS_ENGINE_API VoxelModel final
    {
    public:
        CUBOS_REFLECT;

        ~VoxelModel() = default;

        /// @brief Constructs an empty VoxelModel
        VoxelModel() = default;

        /// @brief Returns the number of grids in the model.
        /// @return Number of grids.
        std::size_t gridCount() const;

        /// @brief Returns the palette of the model.
        /// @return Palette of the model.
        const VoxelPalette& palette() const;

        /// @brief Returns the palette of the model.
        /// @return Palette of the model.
        VoxelPalette& palette();

        /// @brief Returns the grid at the given index.
        /// @param index Index of the grid.
        /// @return Grid at the given index.
        const VoxelGrid& grid(std::size_t index) const;

        /// @brief Returns the grid at the given index.
        /// @param index Index of the grid.
        /// @return Grid at the given index.
        VoxelGrid& grid(std::size_t index);

        /// @brief Returns the position of the grid at the given index.
        /// @param index Index of the grid.
        /// @return Position of the grid at the given index.
        glm::ivec3 gridPosition(std::size_t index) const;

        /// @brief Sets the position of the grid at the given index.
        /// @param index Index of the grid.
        /// @param position Position to set.
        void gridPosition(std::size_t index, glm::ivec3 position);

        /// @brief Loads the voxel model data from the given stream.
        ///
        /// This method reads the voxel model data from a stream in a format compatible with Qubicle files.
        /// The method assumes the following structure in the stream:
        /// https://getqubicle.com/qubicle/documentation/docs/file/qb/
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
        struct QBMatrix
        {
            VoxelGrid grid;      ///< Grid of the matrix.
            glm::ivec3 position; ///< Position of the matrix.
        };
        std::vector<QBMatrix> mMatrices;
        VoxelPalette mPalette;
    };
} // namespace cubos::engine
