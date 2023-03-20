#pragma once

#include <uuid.h>

namespace cubos::engine
{
    /// Handle to an asset. May either be a weak or strong handle.
    /// Weak handles do not guarantee the asset is loaded, while strong handles do.
    ///
    /// @details Assets are identified by their UUID. This is a unique 128-bit number which is
    /// assigned to each asset when it is imported or created. Default constructed handles are
    /// null handles, which are not associated with any asset.
    ///
    /// Serialization:
    /// - can be serialized or deserialized without context, i.e. the UUID is stored directly.
    /// - when deserialized, the handle is always a weak handle.
    class Asset final
    {
    public:
        ~Asset();

        /// Creates a null handle.
        Asset(std::nullptr_t ptr = nullptr);

        /// @param id UUID of the asset.
        Asset(uuids::uuid id);

        /// @param other Handle to copy.
        Asset(const Asset& other);

        /// @param other Handle to move.
        Asset(Asset&& other);

        /// @param other Handle to copy.
        Asset& operator=(const Asset& other);

        /// @param other Handle to move.
        Asset& operator=(Asset&& other);

        /// @returns The UUID of the asset.
        uuids::uuid getId() const;

        /// @returns Whether the handle is null.
        bool isNull() const;

        /// @returns Whether the handle is a strong handle.
        bool isStrong() const;

    private:
        friend class Assets;

        uuids::uuid id; ///< UUID of the asset.
        void* refCount; ///< Void pointer to avoid including <atomic> in the header.
    };
} // namespace cubos::engine
