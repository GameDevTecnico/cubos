/// @file
/// @brief Struct @ref cubos::core::ecs::SystemOptions.
/// @ingroup core-ecs-system

#pragma once

#include <cubos/core/ecs/query/term.hpp>

namespace cubos::core::ecs
{
    /// @brief Contains extra options for a specific system argument.
    /// @ingroup core-ecs-system
    struct SystemOptions
    {
        /// @brief Extra query terms to pass, if it's a query argument.
        std::vector<QueryTerm> queryTerms;

        /// @brief Target entity being observed, if it's an observer argument. -1 otherwise.
        int observedTarget = -1;
    };
} // namespace cubos::core::ecs
