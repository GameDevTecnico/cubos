#pragma once

#include <cubos/core/gl/palette.hpp>

#include <cubos/engine/assets/binary_bridge.hpp>

namespace cubos::engine
{
    /// A bridge to load and save `Palette` assets.
    class PaletteBridge : public BinaryBridgeImpl<core::gl::Palette>
    {
    };
} // namespace cubos::engine
