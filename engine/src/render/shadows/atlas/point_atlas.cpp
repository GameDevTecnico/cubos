#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadows/atlas/point_atlas.hpp>

using cubos::core::gl::Texture2DArrayDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;

CUBOS_REFLECT_IMPL(cubos::engine::PointShadowAtlas)
{
    return core::ecs::TypeBuilder<PointShadowAtlas>("cubos::engine::PointShadowAtlas")
        .withField("configSize", &PointShadowAtlas::configSize)
        .withField("cleared", &PointShadowAtlas::cleared)
        .build();
}

glm::uvec2 cubos::engine::PointShadowAtlas::getSize() const
{
    return mSize;
}

void cubos::engine::PointShadowAtlas::resize(cubos::core::gl::RenderDevice& rd)
{
    mSize = configSize;

    // Prepare texture array description.
    Texture2DArrayDesc cubeDesc{};
    cubeDesc.width = mSize.x;
    cubeDesc.height = mSize.y;
    cubeDesc.size = 6;
    cubeDesc.usage = Usage::Dynamic;

    // Create point shadow atlas texture.
    cubeDesc.format = TextureFormat::Depth32;
    atlas = rd.createTexture2DArray(cubeDesc);
}
