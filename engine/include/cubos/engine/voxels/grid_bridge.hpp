/// @file
/// @brief Class @ref cubos::engine::GridBridge.
/// @ingroup voxels-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Grid assets.
    ///
    /// Grids are composed by their size and their indices.
    /// They're stored as an array of unsigned integers (first 3 equal the grid size, the remainder the grid indexes)
    ///
    /// @ingroup scene-plugin
    class GridBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        GridBridge();

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine
