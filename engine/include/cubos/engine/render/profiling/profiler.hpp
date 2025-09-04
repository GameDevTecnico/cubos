/// @file
/// @brief Class @ref cubos::engine::RenderProfiler.
/// @ingroup render-profiling-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds settings for render performance profiling.
    /// @ingroup render-profiling-plugin
    class CUBOS_ENGINE_API RenderProfiler final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Controls whether profiling is enabled.
        bool profilingEnabled = false;
    };
} // namespace cubos::engine
