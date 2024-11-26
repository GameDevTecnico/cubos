#include <vector>

#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/font/atlas/atlas.hpp>

constexpr double MaxCornerAngle = 3.0; //< Controls the signed distance field type, 3.0 = MSDF.
constexpr int ThreadCount = 4;         //< Amount of threads to use when loading font glyphs.

CUBOS_REFLECT_IMPL(cubos::engine::FontAtlas)
{
    using namespace cubos::core::reflection;

    return Type::create("cubos::engine::FontAtlas")
        .with(ConstructibleTrait::typed<FontAtlas>().withMoveConstructor().build());
}

cubos::engine::FontAtlas::FontAtlas(void* font, double minimumScale, double pixelRange, double miterLimit,
                                    cubos::core::gl::RenderDevice& renderDevice)
{
    std::vector<msdf_atlas::GlyphGeometry> glyphs;
    msdf_atlas::FontGeometry fontGeometry(&glyphs);
    int loaded = fontGeometry.loadCharset(static_cast<msdfgen::FontHandle*>(font), 1.0, msdf_atlas::Charset::ASCII);
    int asciiSize = static_cast<int>(msdf_atlas::Charset::ASCII.size());
    if (loaded != asciiSize)
    {
        CUBOS_WARN("Couldn't load all {} ASCII glyphs, only {} loaded!", asciiSize, loaded);
    }

    for (msdf_atlas::GlyphGeometry& glyph : glyphs)
    {
        glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, MaxCornerAngle, 0);
    }
    msdf_atlas::TightAtlasPacker packer;
    packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
    packer.setMinimumScale(minimumScale);
    packer.setPixelRange(pixelRange);
    packer.setMiterLimit(miterLimit);
    packer.pack(glyphs.data(), static_cast<int>(glyphs.size()));
    int atlasWidth = 0;
    int atlasHeight = 0;
    packer.getDimensions(atlasWidth, atlasHeight);

    msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator,
                                        msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>>
        generator(atlasWidth, atlasHeight);
    msdf_atlas::GeneratorAttributes attributes;
    generator.setAttributes(attributes);
    generator.setThreadCount(ThreadCount);
    generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

    msdfgen::BitmapConstRef<msdfgen::byte, 3> bitmap = generator.atlasStorage();
    for (const msdf_atlas::GlyphGeometry& glyph : glyphs)
    {
        FontGlyph outGlyph{};
        double left;
        double bottom;
        double right;
        double top;
        glyph.getQuadAtlasBounds(left, bottom, right, top);
        outGlyph.texCoordsMin = {static_cast<float>(left / bitmap.width), static_cast<float>(bottom / bitmap.height)};
        outGlyph.texCoordsMax = {static_cast<float>(right / bitmap.width), static_cast<float>(top / bitmap.height)};
        glyph.getQuadPlaneBounds(left, bottom, right, top);
        outGlyph.positionMin = {static_cast<float>(left), static_cast<float>(bottom)};
        outGlyph.positionMax = {static_cast<float>(right), static_cast<float>(top)};
        outGlyph.advance = static_cast<float>(glyph.getAdvance());
        mGlyphs[glyph.getCodepoint()] = outGlyph;
    }
    auto bitmapWidth = static_cast<size_t>(bitmap.width);
    auto bitmapHeight = static_cast<size_t>(bitmap.height);
    std::vector<uint8_t> pixels;
    pixels.reserve(bitmapWidth * bitmapHeight * 4);
    for (size_t i = 0; i < bitmapWidth * bitmapHeight * 3; i += 3)
    {
        pixels.push_back(bitmap.pixels[i]);
        pixels.push_back(bitmap.pixels[i + 1]);
        pixels.push_back(bitmap.pixels[i + 2]);
        pixels.push_back(255);
    }
    core::gl::Texture2DDesc desc{.data = {pixels.data()},
                                 .width = bitmapWidth,
                                 .height = bitmapHeight,
                                 .usage = core::gl::Usage::Default,
                                 .format = core::gl::TextureFormat::RGBA8UNorm};
    mTexture = renderDevice.createTexture2D(desc);
}

std::optional<cubos::engine::FontGlyph> cubos::engine::FontAtlas::requestGlyph(uint32_t unicode) const
{
    auto it = mGlyphs.find(unicode);
    if (it == mGlyphs.end())
    {
        return {};
    }
    return it->second;
}

cubos::core::gl::Texture2D cubos::engine::FontAtlas::texture() const
{
    return mTexture;
}
