#include <cubos/engine/render/profiling/plugin.hpp>
#include <cubos/engine/render/profiling/profiler.hpp>
#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;

void cubos::engine::renderProfilingPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);

    cubos.resource<RenderProfiler>();

    cubos.startupSystem("enable render profiling")
        .after(settingsTag)
        .call([](RenderProfiler& profiler, Settings& settings) {
            profiler.profilingEnabled = settings.getBool("render.profiling", false);
        });
}
