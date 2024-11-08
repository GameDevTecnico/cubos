/// @file
/// @brief Class @ref cubos::engine::AudioBridge.
/// @ingroup audio-plugin

#pragma once

#include <utility>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/audio/audio.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Audio assets.
    ///
    /// Uses the default supported file formats from miniaudio.h, which are WAV, FLAC, and MP3.
    ///
    /// @ingroup audio-plugin
    class AudioBridge : public FileBridge
    {

        std::shared_ptr<cubos::core::al::AudioContext> mContext;

    public:
        /// @brief Constructs a bridge.
        AudioBridge(std::shared_ptr<cubos::core::al::AudioContext> context)
            : FileBridge(core::reflection::reflect<Audio>())
            , mContext(std::move(context))
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine
