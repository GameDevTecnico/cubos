/// @file
/// @brief Class @ref cubos::engine::FontBridge.
/// @ingroup font-plugin

#pragma once

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/font/font.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads @ref Font assets.
    ///
    /// @ingroup font-plugin
    class CUBOS_ENGINE_API FontBridge : public FileBridge
    {
    public:
        FontBridge();

        ~FontBridge() override;

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;

    private:
        void* mFreetypeHandle;
    };
} // namespace cubos::engine
