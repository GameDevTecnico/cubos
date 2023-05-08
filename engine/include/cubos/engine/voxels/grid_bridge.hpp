#pragma once

#include <cubos/core/gl/grid.hpp>

#include <cubos/engine/assets/binary_bridge.hpp>

namespace cubos::engine
{
    /// A bridge to load and save `Grid` assets.
    class GridBridge : public BinaryBridgeImpl<core::gl::Grid>
    {
    };
} // namespace cubos::engine
