#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/profiling/profiler.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderProfiler)
{
    return core::ecs::TypeBuilder<RenderProfiler>("cubos::engine::RenderProfiler").build();
}

void cubos::engine::RenderProfiler::registerResult(std::string tag,
                                                   cubos::core::gl::impl::PipelinedTimer::Result result)
{
    mRegisteredResults.push_back(std::make_pair<>(tag, result));
}

const std::vector<std::pair<std::string, cubos::core::gl::impl::PipelinedTimer::Result>>& cubos::engine::
    RenderProfiler::getRegisteredResults() const
{
    return mRegisteredResults;
}

void cubos::engine::RenderProfiler::clearRegisteredResults()
{
    mRegisteredResults.clear();
}
