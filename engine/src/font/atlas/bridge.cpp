#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <cubos/engine/font/atlas/atlas.hpp>
#include <cubos/engine/font/atlas/bridge.hpp>

bool cubos::engine::FontAtlasBridge::load(Assets& assets, const AnyAsset& handle)
{
    double minimumScale = 24.0;
    double pixelRange = 4.0;
    double miterLimit = 1.0;
    AnyAsset fontAsset{};
    {
        auto meta = assets.readMeta(handle);
        if (auto optMinimumScale = meta->get("minimumScale"))
        {
            minimumScale = std::stod(*optMinimumScale);
        }
        if (auto optPixelRange = meta->get("pixelRange"))
        {
            pixelRange = std::stod(*optPixelRange);
        }
        if (auto optMiterLimit = meta->get("miterLimit"))
        {
            miterLimit = std::stod(*optMiterLimit);
        }
        if (auto optFont = meta->get("font"))
        {
            fontAsset = AnyAsset{*optFont};
        }
        if (fontAsset.isNull())
        {
            CUBOS_ERROR("Could not load FontAtlas: missing font identifier in .meta");
            return false;
        }
    }
    auto assetRead = assets.read<Font>(fontAsset);
    FontAtlas atlas{assetRead->handle(), minimumScale, pixelRange, miterLimit, mRenderDevice};
    assets.store(handle, std::move(atlas));
    return true;
}

bool cubos::engine::FontAtlasBridge::save(const Assets& assets, const AnyAsset& handle)
{
    (void)assets;
    (void)handle;
    CUBOS_ERROR("Saving font atlas is currently unsupported!");
    return false;
}
