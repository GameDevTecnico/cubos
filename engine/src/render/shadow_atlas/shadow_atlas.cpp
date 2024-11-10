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
        .withField("configSize", &ShadowAtlas::configSize)
        .withField("configCubeSize", &ShadowAtlas::configCubeSize)
        .withField("cleared", &ShadowAtlas::cleared)
        .build();
}

glm::uvec2 cubos::engine::ShadowAtlas::getSize() const
{
    return mSize;
}

glm::uvec2 cubos::engine::ShadowAtlas::getCubeSize() const
{
    return mCubeSize;
}

void cubos::engine::ShadowAtlas::resize(cubos::core::gl::RenderDevice& rd)
{
    mSize = configSize;
    mCubeSize = configCubeSize;

    // Prepare texture description.
    Texture2DDesc desc{};
    desc.width = mSize.x;
    desc.height = mSize.y;
    desc.usage = Usage::Dynamic;

    // Create shadow atlas texture.
    desc.format = TextureFormat::Depth32;
    atlas = rd.createTexture2D(desc);

    // Prepare texture array description.
    Texture2DArrayDesc cubeDesc{};
    cubeDesc.width = mCubeSize.x;
    cubeDesc.height = mCubeSize.y;
    cubeDesc.size = 6;
    cubeDesc.usage = Usage::Dynamic;

    // Create shadow cube atlas texture.
    cubeDesc.format = TextureFormat::Depth32;
    cubeAtlas = rd.createTexture2DArray(cubeDesc);
}
