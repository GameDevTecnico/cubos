/// @file
/// @brief Component @ref cubos::engine::RenderTargetDefaults.
/// @ingroup render-defaults-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/memory/opt.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/bloom/bloom.hpp>
#include <cubos/engine/render/deferred_shading/deferred_shading.hpp>
#include <cubos/engine/render/depth/depth.hpp>
#include <cubos/engine/render/g_buffer/g_buffer.hpp>
#include <cubos/engine/render/g_buffer_rasterizer/g_buffer_rasterizer.hpp>
#include <cubos/engine/render/hdr/hdr.hpp>
#include <cubos/engine/render/picker/picker.hpp>
#include <cubos/engine/render/split_screen/split_screen.hpp>
#include <cubos/engine/render/ssao/ssao.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/render/tone_mapping/fxaa.hpp>
#include <cubos/engine/render/tone_mapping/tone_mapping.hpp>

// Export types so that code using the engine shared library can share reflection data,
// otherwise we may end up with e.g. a Opt<cubos::engine::SplitScreen> type in the engine
// and another one in the external code.
namespace cubos::core::memory
{
    CUBOS_ENGINE_EXTERN template class CUBOS_ENGINE_API Opt<cubos::engine::SplitScreen>;
    CUBOS_ENGINE_EXTERN template class CUBOS_ENGINE_API Opt<cubos::engine::Bloom>;
    CUBOS_ENGINE_EXTERN template class CUBOS_ENGINE_API Opt<cubos::engine::SSAO>;
    CUBOS_ENGINE_EXTERN template class CUBOS_ENGINE_API Opt<cubos::engine::RenderPicker>;
} // namespace cubos::core::memory

namespace cubos::engine
{
    /// @brief Component which replaces itself by the default render target components to an entity.
    /// @ingroup render-defaults-plugin
    struct CUBOS_ENGINE_API RenderTargetDefaults
    {
        CUBOS_REFLECT;

        /// @brief Target component.
        RenderTarget target{};

        /// @brief HDR component.
        HDR hdr{};

        /// @brief GBuffer component.
        GBuffer gBuffer{};

        /// @brief Picker component.
        core::memory::Opt<RenderPicker> picker{RenderPicker{}};

        /// @brief Depth component.
        RenderDepth depth{};

        /// @brief GBuffer Rasterizer component.
        GBufferRasterizer gBufferRasterizer{};

        /// @brief SSAO component.
        core::memory::Opt<SSAO> ssao{SSAO{}};

        /// @brief Tone Mapping component.
        ToneMapping toneMapping{};

        /// @brief FXAA component.
        FXAA fxaa{};

        /// @brief Deferred Shading component.
        DeferredShading deferredShading{};

        /// @brief SplitScreen component.
        core::memory::Opt<SplitScreen> splitScreen{SplitScreen{}};

        /// @brief Bloom component.
        core::memory::Opt<Bloom> bloom{Bloom{}};
    };
} // namespace cubos::engine
