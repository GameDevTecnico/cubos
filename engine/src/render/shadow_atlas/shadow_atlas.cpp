#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadow_atlas/shadow_atlas.hpp>

using cubos::core::gl::Texture2DDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;

CUBOS_REFLECT_IMPL(cubos::engine::ShadowAtlas)
{
    return core::ecs::TypeBuilder<ShadowAtlas>("cubos::engine::ShadowAtlas")
        .withField("configSize", &ShadowAtlas::configSize)
        .withField("cleared", &ShadowAtlas::cleared)
        .build();
}

glm::uvec2 cubos::engine::ShadowAtlas::getSize() const
{
    return mSize;
}

void cubos::engine::ShadowAtlas::resize(cubos::core::gl::RenderDevice& rd)
{
    mSize = configSize;

    // Prepare common texture description.
    Texture2DDesc desc{};
    desc.width = mSize.x;
    desc.height = mSize.y;
    desc.usage = Usage::Dynamic;

    // Create shadow atlas texture.
    desc.format = TextureFormat::Depth32;
    atlas = rd.createTexture2D(desc);
}
