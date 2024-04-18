/// @file
/// @brief Resource @ref cubos::engine::RenderMeshPool.
/// @ingroup render-mesh-plugin

#pragma once

#include <queue>
#include <vector>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/render/mesh/vertex.hpp>

namespace cubos::engine
{
    /// @brief Used to allocate render meshes, which can then be rendered using a single draw call.
    ///
    /// Internally, creates a single large vertex buffer, which is divided into buckets.
    /// Allocated meshes must fit within a single bucket. If they don't, they should be split among multiple buckets.
    ///
    /// @ingroup render-mesh-plugin
    class CUBOS_ENGINE_API RenderMeshPool
    {
    public:
        CUBOS_REFLECT;

        /// @brief Identifies a bucket in the pool.
        struct BucketId
        {
            /// @brief Invalid bucket identifier.
            static const BucketId Invalid;

            std::size_t inner{SIZE_MAX}; ///< Bucket identifier.

            /// @brief Compares two bucket identifiers for equality.
            /// @param other Other bucket identifier.
            /// @return Whether the two bucket identifiers are equal.
            bool operator==(const BucketId& other) const = default;
        };

        /// @brief Constructs.
        /// @param renderDevice Render device.
        /// @param bucketCount Bucket count.
        /// @param bucketSize Bucket size (in vertices).
        RenderMeshPool(core::gl::RenderDevice& renderDevice, std::size_t bucketCount, std::size_t bucketSize);

        /// @brief Forbid copy constructor.
        RenderMeshPool(const RenderMeshPool&) = delete;

        /// @brief Forbid copy assignment.
        RenderMeshPool& operator=(const RenderMeshPool&) = delete;

        /// @brief Gets the bucket count.
        /// @return Bucket count.
        std::size_t bucketCount() const;

        /// @brief Gets the bucket size in vertices.
        /// @return Bucket size in vertices.
        std::size_t bucketSize() const;

        /// @brief Gets the number of free buckets.
        /// @return Number of free buckets.
        std::size_t freeBucketCount() const;

        /// @brief Gets the vertex buffer of the pool.
        core::gl::VertexBuffer vertexBuffer() const;

        /// @brief Creates a new bucket, and copies the given vertices into it.
        ///
        /// If the mesh does not fit within a single bucket, then buckets are created and linked as needed.
        ///
        /// @param vertices Pointer to the vertices.
        /// @param count Number of vertices.
        /// @return Bucket identifier, or @ref BucketId::Invalid on failure.
        BucketId allocate(const RenderMeshVertex* vertices, std::size_t count);

        /// @brief Frees the bucket at the given index, and any other buckets linked to it.
        /// @param bucketId Bucket identifier.
        void deallocate(BucketId bucketId);

        /// @brief Gets the number of vertices in the given bucket.
        /// @param bucketId Bucket identifier.
        /// @return Bucket size.
        std::size_t vertexCount(BucketId bucketId) const;

        /// @brief Gets the bucket linked to the given bucket.
        /// @param bucketId Bucket identifier.
        /// @return Bucket identifier, or @ref BucketId::Invalid if there's none.
        BucketId next(BucketId bucketId) const;

    private:
        struct BucketInfo
        {
            /// @brief Number of faces stored in the bucket.
            std::size_t size{0};

            /// @brief Next bucket in the chain.
            BucketId nextId{BucketId::Invalid};
        };

        std::size_t mBucketCount;
        std::size_t mBucketSize;
        core::gl::VertexBuffer mVertexBuffer;

        std::vector<BucketInfo> mBuckets;
        std::queue<BucketId> mFreeBuckets;
    };
} // namespace cubos::engine
