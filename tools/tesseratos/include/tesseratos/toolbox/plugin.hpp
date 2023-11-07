

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
        std::unordered_map<std::string, bool> toolsMap;

        bool isOpen(std::string toolname)
        {
            if (toolsMap.contains(toolname))
            {
                return toolsMap[toolname];
            }
            toolsMap[toolname] = false;
            return false;
        }

        void open(std::string toolname)
        {
            toolsMap[toolname] = true;
        }

        void close(std::string toolname)
        {
            toolsMap[toolname] = false;
        }
    };

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup toolbox-plugin
    void toolboxPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
