#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadows/atlas/spot_atlas.hpp>

using cubos::core::gl::Texture2DDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;

CUBOS_REFLECT_IMPL(cubos::engine::SpotShadowAtlas)
{
    return core::ecs::TypeBuilder<SpotShadowAtlas>("cubos::engine::SpotShadowAtlas")
        .withField("configSize", &SpotShadowAtlas::configSize)
        .withField("cleared", &SpotShadowAtlas::cleared)
        .build();
}

glm::uvec2 cubos::engine::SpotShadowAtlas::getSize() const
{
    return mSize;
}

void cubos::engine::SpotShadowAtlas::resize(cubos::core::gl::RenderDevice& rd)
{
    mSize = configSize;

    // Prepare texture description.
    Texture2DDesc desc{};
    desc.width = mSize.x;
    desc.height = mSize.y;
    desc.usage = Usage::Dynamic;

    // Create spot shadow atlas texture.
    desc.format = TextureFormat::Depth32;
    atlas = rd.createTexture2D(desc);
}
