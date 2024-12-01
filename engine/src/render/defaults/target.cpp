#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/defaults/target.hpp>

template class cubos::core::memory::Opt<cubos::engine::SplitScreen>;
template class cubos::core::memory::Opt<cubos::engine::Bloom>;
template class cubos::core::memory::Opt<cubos::engine::SSAO>;

CUBOS_REFLECT_IMPL(cubos::engine::RenderTargetDefaults)
{
    return core::ecs::TypeBuilder<RenderTargetDefaults>("cubos::engine::RenderTargetDefaults")
        .withField("target", &RenderTargetDefaults::target)
        .withField("hdr", &RenderTargetDefaults::hdr)
        .withField("gBuffer", &RenderTargetDefaults::gBuffer)
        .withField("picker", &RenderTargetDefaults::picker)
        .withField("depth", &RenderTargetDefaults::depth)
        .withField("gBufferRasterizer", &RenderTargetDefaults::gBufferRasterizer)
        .withField("ssao", &RenderTargetDefaults::ssao)
        .withField("toneMapping", &RenderTargetDefaults::toneMapping)
        .withField("deferredShading", &RenderTargetDefaults::deferredShading)
        .withField("splitScreen", &RenderTargetDefaults::splitScreen)
        .withField("bloom", &RenderTargetDefaults::bloom)
        .build();
}
