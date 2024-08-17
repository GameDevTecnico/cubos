#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/render/shadows/directional_caster.hpp>

using cubos::core::gl::Texture2DArrayDesc;
using cubos::core::gl::TextureFormat;
using cubos::core::gl::Usage;

CUBOS_REFLECT_IMPL(cubos::engine::DirectionalShadowCaster)
{
    return core::ecs::TypeBuilder<DirectionalShadowCaster>("cubos::engine::DirectionalShadowCaster")
        .withField("baseSettings", &DirectionalShadowCaster::baseSettings)
        .withField("splitDistances", &DirectionalShadowCaster::splitDistances)
        .withField("maxDistance", &DirectionalShadowCaster::maxDistance)
        .withField("nearDistance", &DirectionalShadowCaster::nearDistance)
        .withField("size", &DirectionalShadowCaster::size)
        .build();
}

void cubos::engine::DirectionalShadowCaster::updateShadowMaps(cubos::core::gl::RenderDevice& rd)
{
    // Clamp number of splits
    if (splitDistances.size() > DirectionalShadowCaster::MaxCascades - 1)
    {
        CUBOS_WARN("Number of splits can't be larger than {}", DirectionalShadowCaster::MaxCascades - 1);
        splitDistances.erase(splitDistances.begin() + DirectionalShadowCaster::MaxCascades - 1, splitDistances.end());
    }

    // Recreate shadow maps if the size or number of splits changed
    if (mSize != size || splitDistances.size() != mSplitDistances.size())
    {
        mSize = size;

        for (auto& [camera, shadowMap] : shadowMaps)
        {
            shadowMap->resize(rd, mSize, static_cast<int>(mSplitDistances.size()) + 1);
        }
    }

    // Set split distances
    mSplitDistances = splitDistances;
}

void cubos::engine::DirectionalShadowCaster::setDefaultSplitDistances(int numCascades)
{
    splitDistances.clear();
    for (int i = 1; i < numCascades; i++)
    {
        splitDistances.push_back(float(i) / float(numCascades));
    }
}

glm::uvec2 cubos::engine::DirectionalShadowCaster::getCurrentSize() const
{
    return mSize;
}

const std::vector<float>& cubos::engine::DirectionalShadowCaster::getCurrentSplitDistances() const
{
    return mSplitDistances;
}

glm::uvec2 cubos::engine::DirectionalShadowCaster::ShadowMap::size() const
{
    return mSize;
}

void cubos::engine::DirectionalShadowCaster::ShadowMap::resize(cubos::core::gl::RenderDevice& rd, glm::uvec2 configSize,
                                                               int numCascades)
{
    mSize = configSize;

    // Prepare texture description.
    Texture2DArrayDesc desc{};
    desc.width = mSize.x;
    desc.height = mSize.y;
    desc.size = static_cast<std::size_t>(numCascades);
    desc.usage = Usage::Dynamic;

    // Create shadow map texture.
    desc.format = TextureFormat::Depth32;
    cascades = rd.createTexture2DArray(desc);
}
