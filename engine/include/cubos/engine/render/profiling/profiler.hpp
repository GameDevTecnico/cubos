/// @file
/// @brief Class @ref cubos::engine::RenderProfiler.
/// @ingroup render-profiling-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief TODO
    /// @ingroup render-profiling-plugin
    class CUBOS_ENGINE_API RenderProfiler final
    {
    public:
        CUBOS_REFLECT;

        void registerResult(std::string tag, cubos::core::gl::impl::PipelinedTimer::Result result);
        const std::vector<std::pair<std::string, cubos::core::gl::impl::PipelinedTimer::Result>>& getRegisteredResults()
            const;
        void clearRegisteredResults();
        bool profilingEnabled = false;

    private:
        std::vector<std::pair<std::string, cubos::core::gl::impl::PipelinedTimer::Result>> mRegisteredResults;
    };
} // namespace cubos::engine
