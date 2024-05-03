/// @file
/// @brief Component @ref cubos::engine::SplitScreen.
/// @ingroup render-split-screen-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which enables splitscreen in a render target.
    /// @ingroup render-split-screen-plugin
    struct CUBOS_ENGINE_API SplitScreen
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
