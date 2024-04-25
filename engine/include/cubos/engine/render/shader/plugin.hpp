/// @dir
/// @brief @ref render-shader-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-shader-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/shader/shader.hpp>

namespace cubos::engine
{
    /// @defgroup render-shader-plugin Shader
    /// @ingroup render-plugins
    /// @brief Adds shader assets to @b CUBOS.
    ///
    /// ## Bridges
    /// - @ref ShaderBridge - registered with the `.vs` extension, loads Vertex @ref Shader assets.
    /// - @ref ShaderBridge - registered with the `.fs` extension, loads Pixel @ref Shader assets.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-shader-plugin
    CUBOS_ENGINE_API void shaderPlugin(Cubos& cubos);
} // namespace cubos::engine
