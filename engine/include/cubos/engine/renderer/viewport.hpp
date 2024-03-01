/// @file
/// @brief Component @ref cubos::engine::Viewport.
/// @ingroup renderer-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines parameters of a viewport, the actual screen space
    /// that will be used by the camera it is attached to. Useful for having multiple camera views
    /// shown on screen.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup renderer-plugin
    struct CUBOS_ENGINE_API Viewport
    {
        CUBOS_REFLECT;

        glm::ivec2 position{0.0F, 0.0F}; //< Top left position of the viewport in pixels
        glm::ivec2 size{0.0F, 0.0F};     //< Size of the viewport in pixels
    };
} // namespace cubos::engine
