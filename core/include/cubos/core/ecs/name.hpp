/// @file
/// @brief Class @ref cubos::core::ecs::Name.
/// @ingroup core-ecs

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::ecs
{
    /// @brief Component which stores the name of an entity.
    /// @ingroup core-ecs
    struct CUBOS_CORE_API Name
    {
        CUBOS_REFLECT;

        /// @brief Name of the entity.
        std::string value;
    };
} // namespace cubos::core::ecs
