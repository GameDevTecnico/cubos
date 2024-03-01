/// @file
/// @brief Class @ref cubos::engine::ShaderBridge.
/// @ingroup render-shader-plugin

#pragma once

#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/render/shader/shader.hpp>

namespace cubos::engine
{
    /// @brief Bridge for loading shader assets.
    /// @ingroup render-shader-plugin
    class CUBOS_ENGINE_API ShaderBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        /// @param renderDevice Render device used to create the shader.
        ShaderBridge(core::gl::RenderDevice& renderDevice)
            : FileBridge(cubos::core::reflection::reflect<cubos::engine::Shader>())
            , mRenderDevice(renderDevice)
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, cubos::core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, cubos::core::memory::Stream& stream) override;

    private:
        core::gl::RenderDevice& mRenderDevice; ///< Render device used to create the shader.
    };
} // namespace cubos::engine
