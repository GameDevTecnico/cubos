/// @dir
/// @brief @ref toolbox-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup toolbox-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-toolbox-plugin Toolbox
    /// @ingroup tesseratos
    /// @brief Adds a resource used to keep track of whether each tool is open or not.
    ///
    ///
    /// ## Resources
    /// - @ref Toolbox

    /// @brief Resource which manages other tools windows.
    class Toolbox final
    {
    public:
        bool isOpen(std::string toolName);

        void open(std::string toolName);

        void close(std::string toolName);

        void doClick(std::string toolName);

        auto begin();

        auto end();

    private:
        std::unordered_map<std::string, bool> mToolsMap;
    };

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup toolbox-plugin
    void toolboxPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
