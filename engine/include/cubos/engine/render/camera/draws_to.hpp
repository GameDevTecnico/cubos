/// @file
/// @brief Relation @ref cubos::engine::DrawsTo.
/// @ingroup render-camera-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Relation which indicates the 'from' entity is a camera that draws to the 'to' target.
    /// @ingroup render-camera-plugin
    struct DrawsTo
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
