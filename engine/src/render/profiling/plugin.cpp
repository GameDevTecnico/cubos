#include <cubos/engine/render/profiling/plugin.hpp>
#include <cubos/engine/render/profiling/profiler.hpp>

using namespace cubos::engine;

void cubos::engine::renderProfilingPlugin(Cubos& cubos)
{
    cubos.resource<RenderProfiler>();
}
