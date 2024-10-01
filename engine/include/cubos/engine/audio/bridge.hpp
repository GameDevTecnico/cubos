/// @file
/// @brief Class @ref cubos::engine::AudioBridge.
/// @ingroup audio-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/audio/audio.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Sound assets.
    ///
    /// Uses the format specified in @ref Audio::loadFrom and @ref Audio::writeTo
    ///
    /// @ingroup audio-plugin
    class AudioBridge : public FileBridge
    {
    public:
        std::shared_ptr<cubos::core::al::MiniaudioContext> mContext;

        /// @brief Constructs a bridge.
        AudioBridge(std::shared_ptr<cubos::core::al::MiniaudioContext> context)
            : FileBridge(core::reflection::reflect<Audio>())
            , mContext(context)
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine
