/// @dir
/// @brief @ref tesseratos-toolbox-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-toolbox-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-toolbox-plugin Toolbox
    /// @ingroup tesseratos
    /// @brief Adds a resource used to keep track of whether each tool is open or not.
    ///
    /// ## Resources
    /// - @ref Toolbox
    ///
    /// ## Dependencies
    /// - @ref imguiPlugin

    /// @brief Resource which manages other tools windows.
    /// @ingroup tesseratos-toolbox-plugin
    class Toolbox final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Checks if the tool with the given name is open.
        /// @param toolName Tool name.
        ///@return Whether the tool is open.
        bool isOpen(const std::string& toolName);

        /// @brief Opens a tool.
        /// @param toolName Tool name.
        void open(const std::string& toolName);

        /// @brief Closes a tool.
        /// @param toolName Tool name.
        void close(const std::string& toolName);

        /// @brief If the given tool is open, closes it. Otherwise, opens it.
        /// @param toolName Tool name.
        void toggle(const std::string& toolName);

        /// @brief Returns the beggining of the map with known tools.
        /// @return Begginng of the map.
        auto begin() const
        {
            return mToolsMap.begin();
        }

        /// @brief Returns the end of the map with known tools.
        /// @return End of the map.
        auto end() const
        {
            return mToolsMap.end();
        }

    private:
        std::unordered_map<std::string, bool> mToolsMap;
    };

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-toolbox-plugin
    void toolboxPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
