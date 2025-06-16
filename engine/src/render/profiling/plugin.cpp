#include <cubos/engine/render/profiling/plugin.hpp>
#include <cubos/engine/render/profiling/profiler.hpp>

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::clearRenderProfilerResultsTag);

void cubos::engine::renderProfilingPlugin(Cubos& cubos)
{
    cubos.resource<RenderProfiler>();

    cubos.tag(clearRenderProfilerResultsTag);

    cubos.system("clear profiler results").tagged(clearRenderProfilerResultsTag).call([](RenderProfiler& profiler) {
        profiler.clearRegisteredResults();
    });
}
