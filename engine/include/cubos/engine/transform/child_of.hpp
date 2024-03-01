/// @file
/// @brief Relation @ref cubos::engine::ChildOf.
/// @ingroup transform-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Tree relation which indicates the 'from' entity is a child of the 'to' entity.
    /// @ingroup transform-plugin
    struct CUBOS_ENGINE_API ChildOf
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
