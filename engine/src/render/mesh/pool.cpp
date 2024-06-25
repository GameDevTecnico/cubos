#include <cstring>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/render/mesh/pool.hpp>

using cubos::core::gl::RenderDevice;
using cubos::core::gl::Usage;
using cubos::core::gl::VertexBuffer;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::Type;
using cubos::engine::RenderMeshPool;

const RenderMeshPool::BucketId RenderMeshPool::BucketId::Invalid{};

CUBOS_REFLECT_IMPL(RenderMeshPool)
{
    return Type::create("cubos::engine::RenderMeshPool").with(ConstructibleTrait::typed<RenderMeshPool>().build());
}

RenderMeshPool::RenderMeshPool(RenderDevice& renderDevice, std::size_t bucketCount, std::size_t bucketSize)
    : mBucketCount{bucketCount}
    , mBucketSize{bucketSize}
{
    CUBOS_ASSERT(bucketSize % 3 == 0, "Bucket size must be a multiple of 3 (to store whole triangles)");

    mVertexBuffer =
        renderDevice.createVertexBuffer(bucketCount * bucketSize * sizeof(RenderMeshVertex), nullptr, Usage::Dynamic);
    if (mVertexBuffer == nullptr)
    {
        mBucketCount = 0;
        CUBOS_ERROR("Failed to create vertex buffer for render mesh pool with {} buckets, each of size {} (vertices)",
                    bucketCount, bucketSize);
    }

    mBuckets.resize(mBucketCount);
    for (std::size_t i = 0; i < mBucketCount; ++i)
    {
        mFreeBuckets.push(BucketId{.inner = i});
    }
}

std::size_t RenderMeshPool::bucketCount() const
{
    return mBucketCount;
}

std::size_t RenderMeshPool::bucketSize() const
{
    return mBucketSize;
}

std::size_t RenderMeshPool::freeBucketCount() const
{
    return mFreeBuckets.size();
}

VertexBuffer RenderMeshPool::vertexBuffer() const
{
    return mVertexBuffer;
}

auto RenderMeshPool::allocate(const RenderMeshVertex* vertices, std::size_t count) -> BucketId
{
    // Check if there are enough buckets first.
    auto neededBucketCount = (count + mBucketSize - 1) / mBucketSize;
    auto freeBucketCount = this->freeBucketCount();
    if (neededBucketCount > freeBucketCount)
    {
        CUBOS_ERROR(
            "Couldn't allocate mesh with {} vertices, which would require {} buckets, while only {} are available. Try "
            "increasing the pool's maximum bucket count (currently {})",
            count, neededBucketCount, freeBucketCount, mBucketCount);
        return BucketId::Invalid;
    }

    BucketId nextId{BucketId::Invalid};

    while (count > 0)
    {
        // Calculate how many of the remaining vertices will be placed on the next bucket.
        auto amount = count < mBucketSize ? count : mBucketSize;

        // Get a free bucket.
        auto bucketId = mFreeBuckets.front();
        mFreeBuckets.pop();

        // Map relevant vertex buffer region and copy the data there.
        // Notice that we do an unsynchronized access - if any operation on the region is still occurring, the behavior
        // will be undefined. Since we popped the first bucket in the free queue, it probably hasn't been used in a long
        // time. Thus, no synchronization here should be safe (and give us a very big performance boost).
        auto* buffer = mVertexBuffer->map(bucketId.inner * mBucketSize * sizeof(RenderMeshVertex),
                                          amount * sizeof(RenderMeshVertex), /*synchronized=*/false);
        memcpy(buffer, vertices, amount * sizeof(RenderMeshVertex));
        mVertexBuffer->unmap();

        // Setup bucket information.
        mBuckets[bucketId.inner].size = amount;
        mBuckets[bucketId.inner].nextId = nextId;
        nextId = bucketId;

        // Advance the given array by the done amount.
        vertices += amount;
        count -= amount;
    }

    return nextId;
}

void RenderMeshPool::deallocate(BucketId bucketId)
{
    while (bucketId != BucketId::Invalid)
    {
        mFreeBuckets.push(bucketId);
        bucketId = mBuckets[bucketId.inner].nextId;
    }
}

std::size_t RenderMeshPool::vertexCount(BucketId bucketId) const
{
    return mBuckets[bucketId.inner].size;
}

auto RenderMeshPool::next(BucketId bucketId) const -> BucketId
{
    return mBuckets[bucketId.inner].nextId;
}
