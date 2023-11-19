/// @dir
/// @brief @ref tesseratos-toolbox-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-toolbox-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-toolbox-plugin Toolbox
    /// @ingroup tesseratos
    /// @brief Adds a resource used to keep track of whether each tool is open or not.
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
        /// @brief When called for the first time for a given name stores a new boolean for it, by default set to false.
        /// @param toolName The tools name
        ///@return Tools associated boolean.
        bool isOpen(std::string toolName);

        /// @brief Opens a tool
        /// @param toolName The tools name
        void open(std::string toolName);

        /// @brief Closes a tool
        /// @param toolName The tools name
        void close(std::string toolName);

        /// @brief Sets the boolean of a tool to its logical opposite state
        /// @param toolName The tools name
        void doClick(std::string toolName);

        /// @brief Returns the beggining of the ToolsMap
        /// @return begginng of the map
        auto begin() const
        {
            return mToolsMap.begin();
        }

        /// @brief Returns the end of the ToolsMap
        /// @return end of the map
        auto end() const
        {
            return mToolsMap.end();
        }

    private:
        std::unordered_map<std::string, bool> mToolsMap;
    };

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-toolbox-plugin
    void toolboxPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
