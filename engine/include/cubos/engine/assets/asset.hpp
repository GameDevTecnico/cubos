/// @file
/// @brief Classes @ref cubos::engine::AnyAsset and @ref cubos::engine::Asset.
/// @ingroup assets-plugin

#pragma once

#include <uuid.h>

namespace cubos::core::data
{
    class Serializer;
    class Deserializer;
} // namespace cubos::core::data

namespace cubos::engine
{
    template <typename T>
    class Asset;

    /// @brief Handle to an asset of any type. May either be weak or strong.
    /// Weak handles do not guarantee the asset is loaded, while strong handles do.
    ///
    /// Assets are identified by their UUID. This is a unique 128-bit number which is
    /// assigned to each asset when it is imported or created. Default constructed handles are
    /// null handles, which are not associated with any asset.
    ///
    /// Serialization:
    /// - can be serialized or deserialized without context, i.e. the UUID is stored directly.
    /// - when deserialized, the handle is always a weak handle.
    ///
    /// @ingroup assets-plugin
    class AnyAsset
    {
    public:
        ~AnyAsset();

        /// @brief Constructs a null handle.
        AnyAsset(std::nullptr_t ptr = nullptr);

        /// @brief Constructs a weak handle.
        /// @param id UUID of the asset.
        AnyAsset(uuids::uuid id);

        /// @brief Constructs a weak handle. If the string is not a valid UUID, the handle will be
        /// null.
        /// @param str String representation of the UUID.
        AnyAsset(std::string_view str);

        /// @brief Constructs a copy of the given handle.
        /// @param other Handle to copy.
        AnyAsset(const AnyAsset& other);

        /// @brief Constructs a handle from the given handle.
        /// @param other Handle to move.
        AnyAsset(AnyAsset&& other) noexcept;

        /// @brief Overwrites this handle with a copy of the given handle.
        /// @param other Handle to copy.
        AnyAsset& operator=(const AnyAsset& other);

        /// @brief Overwrites this handle with the given handle.
        /// @param other Handle to move.
        AnyAsset& operator=(AnyAsset&& other) noexcept;

        /// @brief Gets the version of the asset last seen by this handle.
        /// @return Asset version.
        int getVersion() const;

        /// @brief Gets the UUID of the asset.
        /// @return Asset UUID.
        uuids::uuid getId() const;

        /// @brief Checks if the handle is null.
        /// @return Whether the handle is null.
        bool isNull() const;

        /// @brief Checks if the handle is strong.
        /// @return Whether the handle is strong.
        bool isStrong() const;

        /// @brief Makes this handle a weak handle, decreasing the asset's reference count if it
        /// was strong.
        void makeWeak();

        /// @brief Converts this handle to a handle of a specific type.
        /// @tparam T Type of the asset.
        /// @return Handle to the same asset, but of the specified type.
        template <typename T>
        inline operator Asset<T>() const;

        void serialize(core::data::Serializer& ser, const char* name) const;
        void deserialize(core::data::Deserializer& des);

    private:
        friend class Assets;

        /// @brief Increments the reference count of the asset.
        void incRef() const;

        /// @brief Decrements the reference count of the asset.
        void decRef() const;

        uuids::uuid mId; ///< UUID of the asset.
        void* mRefCount; ///< Void pointer to avoid including <atomic> in the header.
        int mVersion;    ///< Last known version of the asset.
    };

    /// @brief Handle to an asset of a specific type.
    /// @see AnyAsset
    /// @tparam T Type of the asset.
    /// @ingroup assets-plugin
    template <typename T>
    class Asset : public AnyAsset
    {
    public:
        using AnyAsset::AnyAsset;

        /// @brief Constructs a generic handle version of this handle.
        /// @return Generic handle to the same asset.
        inline AnyAsset toAny() const
        {
            return AnyAsset(*this);
        }

        /// @brief Overwrites this handle with a copy of the given handle.
        /// @param other Handle to copy.
        /// @return Reference to this object, for chaining.
        inline Asset<T>& operator=(const AnyAsset& other)
        {
            AnyAsset::operator=(other);
            return *this;
        }

        /// @brief Overwrites this handle with the given handle.
        /// @param other Handle to move.
        /// @return Reference to this object, for chaining.
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
