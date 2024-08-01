#include <unordered_map>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/endianness.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/palette.hpp>

using cubos::core::memory::fromBigEndian;
using cubos::core::memory::Stream;
using cubos::core::memory::toBigEndian;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(VoxelGrid)
{
    using namespace cubos::core::reflection;

    return Type::create("cubos::engine::VoxelGrid")
        .with(ConstructibleTrait::typed<VoxelGrid>().withDefaultConstructor().withMoveConstructor().build());
}

VoxelGrid::VoxelGrid(const glm::uvec3& size)
{
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        mSize = {1, 1, 1};
        CUBOS_WARN("Grid size must be at least 1 in each dimension: was {}, defaulting to {}", size, mSize);
    }
    else
    {
        mSize = size;
    }

    mIndices.resize(
        static_cast<std::size_t>(mSize.x) * static_cast<std::size_t>(mSize.y) * static_cast<std::size_t>(mSize.z), 0);
}

VoxelGrid::VoxelGrid(const glm::uvec3& size, const std::vector<uint16_t>& indices)
{
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        mSize = {1, 1, 1};
        CUBOS_WARN("Grid size must be at least 1 in each dimension: was {}, defaulting to {}", size, mSize);
    }
    else if (indices.size() !=
             static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y) * static_cast<std::size_t>(size.z))
    {
        CUBOS_WARN("Grid size and indices size mismatch: was {}, indices size is {}", size, indices.size());
        mSize = {1, 1, 1};
    }
    else
    {
        mSize = size;
    }

    mIndices = indices;
}

VoxelGrid::VoxelGrid(VoxelGrid&& other) noexcept
    : mSize(other.mSize)
{
    new (&mIndices) std::vector<uint16_t>(std::move(other.mIndices));
}

VoxelGrid& VoxelGrid::operator=(VoxelGrid&& other) noexcept
{
    if (this != &other)
    {
        mSize = other.mSize;
        mIndices = std::move(other.mIndices);

        other.mSize = {1, 1, 1};
        other.mIndices.clear();
        other.mIndices.resize(1, 0);
    }
    return *this;
}

VoxelGrid::VoxelGrid(const VoxelGrid& other)
    : mSize(other.mSize)
    , mIndices(other.mIndices)
{
}

VoxelGrid::VoxelGrid()
{
    mSize = {1, 1, 1};
    mIndices.resize(1, 0);
}

VoxelGrid& VoxelGrid::operator=(const VoxelGrid& rhs) = default;

void VoxelGrid::setSize(const glm::uvec3& size)
{
    if (size == mSize)
    {
        return;
    }
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        CUBOS_WARN("Grid size must be at least 1 in each dimension: preserving original dimensions (tried to set to "
                   "({}, {}, {}))",
                   size.x, size.y, size.z);
        return;
    }

    mSize = size;
    mIndices.clear();
    mIndices.resize(
        static_cast<std::size_t>(mSize.x) * static_cast<std::size_t>(mSize.y) * static_cast<std::size_t>(mSize.z), 0);
}

const glm::uvec3& VoxelGrid::size() const
{
    return mSize;
}

void VoxelGrid::clear()
{
    for (auto& i : mIndices)
    {
        i = 0;
    }
}

uint16_t VoxelGrid::get(const glm::ivec3& position) const
{
    assert(position.x >= 0 && position.x < static_cast<int>(mSize.x));
    assert(position.y >= 0 && position.y < static_cast<int>(mSize.y));
    assert(position.z >= 0 && position.z < static_cast<int>(mSize.z));
    auto index = position.x + position.y * static_cast<int>(mSize.x) + position.z * static_cast<int>(mSize.x * mSize.y);
    return mIndices[static_cast<std::size_t>(index)];
}

void VoxelGrid::set(const glm::ivec3& position, uint16_t mat)
{
    assert(position.x >= 0 && position.x < static_cast<int>(mSize.x));
    assert(position.y >= 0 && position.y < static_cast<int>(mSize.y));
    assert(position.z >= 0 && position.z < static_cast<int>(mSize.z));
    auto index = position.x + position.y * static_cast<int>(mSize.x) + position.z * static_cast<int>(mSize.x * mSize.y);
    mIndices[static_cast<std::size_t>(index)] = mat;
}

bool VoxelGrid::convert(const VoxelPalette& src, const VoxelPalette& dst, float minSimilarity)
{
    // Find the mappings for every material in the source palette.
    std::unordered_map<uint16_t, uint16_t> mappings;
    for (uint16_t i = 0; i <= src.size(); ++i)
    {
        uint16_t j = dst.find(src.get(i));
        if (src.get(i).similarity(dst.get(j)) >= minSimilarity)
        {
            mappings[i] = j;
        }
    }

    // Check if the mappings are complete for every material being used in the grid.
    for (uint32_t i = 0; i < mSize.x * mSize.y * mSize.z; ++i)
    {
        if (mappings.find(mIndices[i]) == mappings.end())
        {
            return false;
        }
    }

    // Apply the mappings.
    for (uint32_t i = 0; i < mSize.x * mSize.y * mSize.z; ++i)
    {
        mIndices[i] = mappings[mIndices[i]];
    }

    return true;
}

bool VoxelGrid::loadFrom(Stream& stream)
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
    if (stream.read(&x, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        CUBOS_ERROR("Failed to load voxel grid, unexpected end of file");
        return false;
    }
    if (stream.read(&y, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        CUBOS_ERROR("Failed to load voxel grid, unexpected end of file");
        return false;
    }
    if (stream.read(&z, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        CUBOS_ERROR("Failed to load voxel grid, unexpected end of file");
        return false;
    }
    x = fromBigEndian(x);
    y = fromBigEndian(y);
    z = fromBigEndian(z);
    mSize = glm::uvec3(x, y, z);

    mIndices.clear();
    for (uint32_t i = 0; i < x * y * z; i++)
    {
        uint16_t idx;
        if (stream.read(&idx, sizeof(uint16_t)) != sizeof(uint16_t))
        {
            CUBOS_ERROR("Failed to load voxel grid, unexpected end of file");
            return false;
        }
        idx = fromBigEndian(idx);
        mIndices.push_back(idx);
    }
    return true;
}

bool VoxelGrid::writeTo(Stream& stream) const
{
    uint32_t x = toBigEndian(mSize.x);
    uint32_t y = toBigEndian(mSize.y);
    uint32_t z = toBigEndian(mSize.z);
    if (stream.write(&x, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        CUBOS_ERROR("Failed to save voxel grid, couldn't write it to stream");
        return false;
    }
    if (stream.write(&y, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        CUBOS_ERROR("Failed to save voxel grid, couldn't write it to stream");
        return false;
    }
    if (stream.write(&z, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        CUBOS_ERROR("Failed to save voxel grid, couldn't write it to stream");
        return false;
    }

    for (const auto& indice : mIndices)
    {
        uint16_t idx = toBigEndian(indice);
        if (stream.write(&idx, sizeof(uint16_t)) != sizeof(uint16_t))
        {
            CUBOS_ERROR("Failed to save voxel grid, couldn't write it to stream");
            return false;
        }
    }
    return true;
}
