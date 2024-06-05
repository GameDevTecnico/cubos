/// @file
/// @brief Component @ref tesseratos::MenuBarItem.
/// @ingroup tesseratos-menu-bar-plugin

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace tesseratos
{
    /// @brief Component representing a menu bar item.
    ///
    /// Adds an item to the menu bar at the top of the screen.
    ///
    /// @ingroup tesseratos-menu-bar-plugin
    struct MenuBarItem
    {
        CUBOS_REFLECT;

        /// @brief Item name.
        std::string name{"unnamed"};

        /// @brief Item order priority, lower values are displayed first.
        ///
        /// Ties are broken by the item's alphabetical order.
        int order{0};
    };
} // namespace tesseratos
