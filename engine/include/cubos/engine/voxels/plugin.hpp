#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace cubos::engine
{
    /// Plugin which adds grid and palette asset bridges to CUBOS.
    ///
    /// @details Registers asset bridges for files with `.grd` and `.pal` extensions, which load
    /// assets of the `Grid` and `Palette` types, respectively. Both assets are stored as binary
    /// serialized files.
    ///
    /// Dependencies:
    /// - `assetsPlugin`
    ///
    /// @param cubos CUBOS. main class.
    void voxelsPlugin(Cubos& cubos);
} // namespace cubos::engine
