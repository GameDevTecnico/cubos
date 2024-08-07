/// @file
/// @brief Class @ref cubos::engine::VoxelModelBridge.
/// @ingroup voxels-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref VoxelModel assets.
    ///
    /// Uses the format specified in @ref VoxelModel::loadFrom and @ref VoxelModel::writeTo
    ///
    /// @ingroup scene-plugin
    class VoxelModelBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        VoxelModelBridge();

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine