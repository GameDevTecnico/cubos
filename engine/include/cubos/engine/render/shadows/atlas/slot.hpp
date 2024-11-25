/// @file
/// @brief Resource @ref cubos::engine::ShadowMapSlot.
/// @ingroup render-shadow-atlas-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Slot for a shadow map in the shadow atlas.
    struct ShadowMapSlot
    {
        glm::vec2 size;   ///< Shadow map size, in normalized coordinates.
        glm::vec2 offset; ///< Shadow map offset, in normalized coordinates.
        int casterId;     ///< Id of the shadow caster (-1 if none).

        /// @brief Constructs.
        /// @param size Shadow map size, in normalized coordinates.
        /// @param offset Shadow map offset, in normalized coordinates.
        /// @param casterId Id of the shadow caster (-1 if none).
        ShadowMapSlot(glm::vec2 size, glm::vec2 offset, int casterId)
            : size(size)
            , offset(offset)
            , casterId(casterId) {};
    };
} // namespace cubos::engine
