#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/profiling/profiler.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::RenderProfiler)
{
    return core::ecs::TypeBuilder<RenderProfiler>("cubos::engine::RenderProfiler").build();
}
