/// @file
/// @brief Class @ref cubos::engine::PaletteBridge.
/// @ingroup voxels-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Palette assets.
    ///
    /// Uses the format specified in @ref VoxelPalette::loadFrom and @ref VoxelPalette::writeTo
    ///
    /// @ingroup scene-plugin
    class VoxelPaletteBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        VoxelPaletteBridge();

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine