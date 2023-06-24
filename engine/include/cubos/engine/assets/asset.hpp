#pragma once

#include <uuid.h>

namespace cubos::core::old::data
{
    class Serializer;
    class Deserializer;
} // namespace cubos::core::old::data

namespace cubos::engine
{
    template <typename T>
    class Asset;

    /// Handle to an asset - of any type. May either be a weak or strong handle.
    /// Weak handles do not guarantee the asset is loaded, while strong handles do.
    ///
    /// @details Assets are identified by their UUID. This is a unique 128-bit number which is
    /// assigned to each asset when it is imported or created. Default constructed handles are
    /// null handles, which are not associated with any asset.
    ///
    /// Serialization:
    /// - can be serialized or deserialized without context, i.e. the UUID is stored directly.
    /// - when deserialized, the handle is always a weak handle.
    class AnyAsset
    {
    public:
        ~AnyAsset();

        /// Creates a null handle.
        AnyAsset(std::nullptr_t ptr = nullptr);

        /// @param id UUID of the asset.
        AnyAsset(uuids::uuid id);

        /// If the string is not a valid UUID, the handle will be null.
        /// @param str String representation of the UUID.
        AnyAsset(std::string_view str);

        /// @param other Handle to copy.
        AnyAsset(const AnyAsset& other);

        /// @param other Handle to move.
        AnyAsset(AnyAsset&& other) noexcept;

        /// @param other Handle to copy.
        AnyAsset& operator=(const AnyAsset& other);

        /// @param other Handle to move.
        AnyAsset& operator=(AnyAsset&& other) noexcept;

        /// @returns The last known version of the asset.
        int getVersion() const;

        /// @returns The UUID of the asset.
        uuids::uuid getId() const;

        /// @returns Whether the handle is null.
        bool isNull() const;

        /// @returns Whether the handle is a strong handle.
        bool isStrong() const;

        /// Makes this handle a weak handle.
        void makeWeak();

        /// Converts this handle to a handle of a specific type.
        /// @tparam T Type of the asset.
        /// @returns A handle to the same asset, but of the specified type.
        template <typename T>
        inline operator Asset<T>() const;

        /// @param ser The serializer to use.
        /// @param name The name of the object.
        void serialize(core::old::data::Serializer& ser, const char* name) const;

        /// @param des The deserializer to use.
        void deserialize(core::old::data::Deserializer& des);

    private:
        friend class Assets;

        /// Increments the reference count of the asset.
        void incRef() const;

        /// Decrements the reference count of the asset.
        void decRef() const;

        uuids::uuid mId; ///< UUID of the asset.
        void* mRefCount; ///< Void pointer to avoid including <atomic> in the header.
        int mVersion;    ///< Last known version of the asset.
    };

    /// Handle to an asset of a specific type.
    /// @see AnyAsset
    /// @tparam T Type of the asset.
    template <typename T>
    class Asset : public AnyAsset
    {
    public:
        using AnyAsset::AnyAsset;

        /// @returns A generic handle to the same asset.
        inline AnyAsset toAny() const
        {
            return AnyAsset(*this);
        }

        /// @param other Handle to copy.
        inline Asset<T>& operator=(const AnyAsset& other)
        {
            AnyAsset::operator=(other);
            return *this;
        }

        /// @param other Handle to move.
        inline Asset<T>& operator=(AnyAsset&& other) noexcept
        {
            AnyAsset::operator=(std::move(other));
            return *this;
        }
    };

    // Implementation.

    template <typename T>
    inline AnyAsset::operator Asset<T>() const
    {
        Asset<T> asset;
        asset.mId = mId;
        asset.mRefCount = mRefCount;
        asset.mVersion = mVersion;
        asset.incRef();
        return asset;
    }
} // namespace cubos::engine
