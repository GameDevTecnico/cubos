/// @file
/// @brief Class @ref cubos::engine::PaletteBridge.
/// @ingroup voxels-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Palette assets.
    ///
    /// Palettes are composed by n materials. They're stored as JSON arrays
    ///
    /// @ingroup scene-plugin
    class PaletteBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        PaletteBridge();

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine