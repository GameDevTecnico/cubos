/// @file
/// @brief Class @ref cubos::engine::GridBridge.
/// @ingroup voxels-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Grid assets.
    ///
    /// Uses the format specified in @ref VoxelGrid::loadFrom and @ref VoxelGrid::writeTo
    ///
    /// @ingroup voxels-plugin
    class VoxelGridBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        VoxelGridBridge();

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine
