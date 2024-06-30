/// @file
/// @brief Class @ref cubos::engine::ImageBridge.
/// @ingroup image-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/image/image.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Image assets.
    ///
    /// @ingroup image-plugin
    class CUBOS_ENGINE_API ImageBridge : public FileBridge
    {
    public:
        ImageBridge()
            : FileBridge(core::reflection::reflect<Image>())
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine
