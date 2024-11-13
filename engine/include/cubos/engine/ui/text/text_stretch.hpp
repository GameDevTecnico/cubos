/// @file
/// @brief Component @ref cubos::engine::UITextStretch.
/// @ingroup ui-text-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes a UI element fit the text it has.
    /// @ingroup ui-text-plugin
    struct CUBOS_ENGINE_API UITextStretch
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
