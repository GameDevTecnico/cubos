/// @file
/// @brief Component @ref cubos::engine::CollisionLayers.
/// @ingroup collisions-plugin

#pragma once

#include <cstdint>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component that contains the collision layers where the collider appears in.
    /// Check also @ref CollisionMask.
    /// @ingroup collisions-plugin
    struct CUBOS_ENGINE_API CollisionLayers
    {
        CUBOS_REFLECT;

        /// @brief Layers of the collider.
        ///
        /// There are 32 layers, from 0 to 31. A collider can belong to any number of layers.
        ///
        /// Internally, the layers go from right to left, meaning, layer 0 is the least significant bit and layer 31 the
        /// most significant bit. Any type of number can be used to manually set the layers, just make sure the binary
        /// representation has all the bits corresponding to the wanted layers set to 1.
        ///
        /// By default, colliders appear only in layer 0.
        uint32_t value = 0x00000001;

        /// @brief Based on `newValue`, enables or disables the specified `layerNumber`.
        /// @param layerNumber Layer number between 0 and 31.
        /// @param newValue `true` enables the layer. `false` disables the layer.
        void setLayerValue(unsigned int layerNumber, bool newValue);

        /// @brief Returns whether or not the specified `layerNumber` is enabled.
        /// @param layerNumber Layer number between 0 and 31.
        bool getLayerValue(unsigned int layerNumber) const;
    };
} // namespace cubos::engine
