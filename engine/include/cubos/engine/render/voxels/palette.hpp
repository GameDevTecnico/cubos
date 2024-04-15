/// @file
/// @brief Resource @ref cubos::engine::RenderPalette.
/// @ingroup render-voxels-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/palette.hpp>

namespace cubos::engine
{
    /// @brief Resource which identifies the currently active palette of the scene.
    /// @ingroup render-voxels-plugin
    struct CUBOS_ENGINE_API RenderPalette
    {
        CUBOS_REFLECT;

        /// @brief Handle to the palette asset.
        Asset<VoxelPalette> asset{};
    };
} // namespace cubos::engine
