/// @file
/// @brief Struct @ref cubos::engine::Glyph.
/// @ingroup font-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Struct that holds glyph data such as texcoords, position offsets and advance.
    ///
    /// @ingroup font-plugin
    struct CUBOS_ENGINE_API FontGlyph
    {
        CUBOS_REFLECT;

        glm::vec2 texCoordsMin;
        glm::vec2 texCoordsMax;

        glm::vec2 positionMin;
        glm::vec2 positionMax;

        float advance;
    };
} // namespace cubos::engine
