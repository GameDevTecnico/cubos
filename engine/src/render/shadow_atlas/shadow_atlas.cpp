#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadow_atlas/shadow_atlas.hpp>

using cubos::core::gl::Texture2DArrayDesc;
using cubos::core::gl::Texture2DDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;

CUBOS_REFLECT_IMPL(cubos::engine::ShadowAtlas)
{
    return core::ecs::TypeBuilder<ShadowAtlas>("cubos::engine::ShadowAtlas")
        .withField("configSpotAtlasSize", &ShadowAtlas::configSpotAtlasSize)
        .withField("configPointAtlasSize", &ShadowAtlas::configPointAtlasSize)
        .withField("cleared", &ShadowAtlas::cleared)
        .build();
}

glm::uvec2 cubos::engine::ShadowAtlas::getSpotAtlasSize() const
{
    return mSpotAtlasSize;
}

glm::uvec2 cubos::engine::ShadowAtlas::getPointAtlasSize() const
{
    return mPointAtlasSize;
}

void cubos::engine::ShadowAtlas::resize(cubos::core::gl::RenderDevice& rd)
{
    mSpotAtlasSize = configSpotAtlasSize;
    mPointAtlasSize = configPointAtlasSize;

    // Prepare texture description.
    Texture2DDesc desc{};
    desc.width = mSpotAtlasSize.x;
    desc.height = mSpotAtlasSize.y;
    desc.usage = Usage::Dynamic;

    // Create spot shadow atlas texture.
    desc.format = TextureFormat::Depth32;
    spotAtlas = rd.createTexture2D(desc);

    // Prepare texture array description.
    Texture2DArrayDesc cubeDesc{};
    cubeDesc.width = mPointAtlasSize.x;
    cubeDesc.height = mPointAtlasSize.y;
    cubeDesc.size = 6;
    cubeDesc.usage = Usage::Dynamic;

    // Create point shadow atlas texture.
    cubeDesc.format = TextureFormat::Depth32;
    pointAtlas = rd.createTexture2DArray(cubeDesc);
}
