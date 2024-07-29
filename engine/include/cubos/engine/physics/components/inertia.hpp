/// @file
/// @brief Component @ref cubos::engine::Inertia.
/// @ingroup physics-plugin

#pragma once

#include <glm/mat3x3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines the inertia of a body.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Inertia
    {
        CUBOS_REFLECT;

        glm::mat3 inertia;
        glm::mat3 inverseInertia;
        bool selfUpdate = false;
    };
} // namespace cubos::engine
