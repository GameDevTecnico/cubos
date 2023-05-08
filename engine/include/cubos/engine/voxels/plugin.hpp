#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace cubos::engine
{
    /// Plugin which adds grid and palette asset bridges to CUBOS.
    ///
    /// @details Registers the `GridBridge` and `PaletteBridge` asset bridges with the `.grd` and
    /// `.pal` extensions, which load assets of the `Grid` and `Palette` types, respectively.
    ///
    /// Dependencies:
    /// - `assetsPlugin`
    ///
    /// @param cubos CUBOS. main class.
    void voxelsPlugin(Cubos& cubos);
} // namespace cubos::engine
