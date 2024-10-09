/// @dir
/// @brief @ref toolbox-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup toolbox-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/tools/toolbox/toolbox.hpp>

namespace cubos::engine
{
    /// @defgroup toolbox-tool-plugin Toolbox
    /// @ingroup tool-plugins
    /// @brief Allows toggling the visibility of other tools.
    ///
    /// Most tools in the @ref tool-plugins module rely on the @ref Toolbox to keep track of whether they are open or
    /// not.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup toolbox-tool-plugin
    CUBOS_ENGINE_API void toolboxPlugin(Cubos& cubos);
} // namespace cubos::engine
