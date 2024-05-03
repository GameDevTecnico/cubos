/// @file
/// @brief Relation @ref cubos::engine::DrawsTo.
/// @ingroup render-camera-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Relation which indicates the 'from' entity is a camera that draws to the 'to' target.
    /// @ingroup render-camera-plugin
    struct CUBOS_ENGINE_API DrawsTo
    {
        CUBOS_REFLECT;

        /// @brief Offset of the viewport, in normalized coordinates.
        glm::vec2 viewportOffset = {0, 0};

        /// @brief Size of the viewport, in normalized coordinates.
        glm::vec2 viewportSize = {1, 1};
    };
} // namespace cubos::engine
