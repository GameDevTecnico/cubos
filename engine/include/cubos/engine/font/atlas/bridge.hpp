/// @file
/// @brief Class @ref cubos::engine::FontAtlasBridge.
/// @ingroup font-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/assets/bridge.hpp>
#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/font/atlas/atlas.hpp>
#include <cubos/engine/font/font.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads @ref FontAtlas assets.
    ///
    /// @ingroup font-plugin
    class CUBOS_ENGINE_API FontAtlasBridge : public AssetBridge
    {
    public:
        FontAtlasBridge(cubos::core::gl::RenderDevice& renderDevice)
            : AssetBridge(core::reflection::reflect<FontAtlas>(), false)
            , mRenderDevice(renderDevice)
        {
        }

    protected:
        bool load(Assets& assets, const AnyAsset& handle) override;
        bool save(const Assets& assets, const AnyAsset& handle) override;

    private:
        cubos::core::gl::RenderDevice& mRenderDevice;
    };
} // namespace cubos::engine
