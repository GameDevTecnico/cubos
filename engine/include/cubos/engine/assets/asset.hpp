/// @file
/// @brief Classes @ref cubos::engine::AnyAsset and @ref cubos::engine::Asset.
/// @ingroup assets-plugin

#pragma once

#include <uuid.h>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::data::old
{
    class Serializer;
    class Deserializer;
} // namespace cubos::core::data::old

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
        CUBOS_REFLECT;

        /// @brief Avoid using this field, use @ref getId() instead.
        /// @todo This was added as a dirty fix for #692, should be removed once the issue is fixed.
        uuids::uuid reflectedId;

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

        /// @brief Equality operator for comparing two AnyAsset objects.
        /// @param other The other AnyAsset to compare.
        /// @return True if the two AnyAsset objects have the same UUID , otherwise false.
        bool operator==(const AnyAsset& other) const;

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

        void serialize(core::data::old::Serializer& ser, const char* name) const;
        void deserialize(core::data::old::Deserializer& des);

    protected:
        /// @brief Constructs a type with the given name, constructible trait and UUID field.
        ///
        /// Added so that typed asset handles don't duplicate the existing reflection code of the
        /// base class.
        ///
        /// @param name Type name.
        static core::reflection::Type& makeType(std::string name);

    private:
        friend class Assets;

        /// @brief Increments the reference count of the asset.
        void incRef() const;

        /// @brief Decrements the reference count of the asset.
        void decRef() const;

        uuids::uuid mId; ///< UUID of the asset.
        void* mRefCount; ///< Void pointer to avoid including `<atomic>` in the header.
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
        CUBOS_REFLECT;

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
        asset.reflectedId = reflectedId;
        asset.mId = mId;
        asset.mRefCount = mRefCount;
        asset.mVersion = mVersion;
        asset.incRef();
        return asset;
    }

    CUBOS_REFLECT_TEMPLATE_IMPL((typename T), (Asset<T>))
    {
        return AnyAsset::makeType("cubos::engine::Asset<" + core::reflection::reflect<T>().name() + ">");
    }
} // namespace cubos::engine
