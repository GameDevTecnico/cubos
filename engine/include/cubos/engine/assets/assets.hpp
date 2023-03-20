#pragma once

#include <cubos/engine/assets/bridge.hpp>
#include <cubos/engine/assets/meta.hpp>

#include <cubos/core/memory/type_map.hpp>

#include <memory>
#include <string>

namespace cubos::engine
{
    /// Manages all assets. Responsible for loading and unloading assets, storing them in memory,
    /// and providing access to them.
    ///
    /// @details Assets are all identified through `Asset` handles.
    class Assets final
    {
    public:
        /// Possible statuses for an asset.
        enum class Status
        {
            Unknown, ///< No metadata is associated with the asset.
            Loading, ///< The asset is being loaded.
            Loaded,  ///< The asset is loaded.
        };

        Assets() = default;
        Assets(const Assets&) = delete;
        Assets& operator=(const Assets&) = delete;
        ~Assets() = default;

        /// Loads all metadata from the virtual filesystem, in the given path.
        /// If the path is a directory, it will be recursively searched for metadata files.
        /// @param path The path to load metadata from.
        void loadMeta(std::string_view path);

        /// Registers a new bridge for the given extension.
        /// @param extension The extension to register the bridge for.
        /// @param bridge The bridge to register.
        void registerBridge(std::string extension, std::unique_ptr<AssetBridge> bridge);

        /// Gets read-only access to the metadata associated with the given handle.
        /// Aborts if the asset is unknown.
        /// @param handle The handle to get the metadata for.
        /// @returns Reference to the metadata.
        const AssetMeta& meta(Asset handle) const;

        /// Gets read-write access to the metadata associated with the given handle.
        /// If the asset is unknown, an empty metadata object is returned.
        /// @param handle The handle to get the metadata for.
        /// @returns Reference to the metadata.
        AssetMeta& meta(Asset handle);

        /// Loads the asset with the given handle, upgrading the handle to a strong one.
        /// This method doesn't block, and the asset may not yet be loaded when it returns.
        /// If the manager is unable to load the asset, a null handle is returned. If an error
        /// occurs while loading the asset, it will only fail in `get` or be visible through
        /// `status`.
        /// @param handle The handle to load the asset for.
        /// @returns A strong handle to the asset.
        Asset load(Asset handle);

        /// Saves changes made to the asset with the given handle. This blocks until the asset is
        /// saved. If the asset cannot be saved - for example, if there's no associated capable
        /// bridge - this method will return false.
        /// @param handle The handle to save the asset for.
        /// @param onlyMeta Whether to only save the metadata, not the asset data.
        /// @returns True if the operation successful, false otherwise.
        bool save(Asset handle, bool onlyMeta = false);

        /// Checks the status of the asset with the given handle.
        /// @param handle The handle to check the status for.
        /// @returns The status of the asset.
        Status status(Asset handle) const;

        /// Updates the given handle to the latest version of the asset.
        /// Can be used to provide asset hot-reloading.
        /// @param handle The handle to update.
        /// @returns Whether the version was updated.
        bool update(Asset& handle);

        /// Creates a new asset with the given data (and empty metadata) and returns an handle to
        /// it. A random UUID is generated for the asset.
        /// @tparam T The type of the asset data.
        /// @param data The asset data to store.
        /// @returns A strong handle to the newly created asset.
        template <typename T>
        inline Asset create(T data)
        {
            // TODO
            abort();
        }

        /// Stores the given asset data in memory, associated with the given handle.
        /// If an asset with the same handle already exists, it will be replaced.
        /// If no metadata is associated with the handle, an empty one will be created.
        /// This increases the asset's version.
        /// @tparam T The type of the asset data.
        /// @param handle The handle to associate the asset with.
        /// @param data The asset data to store.
        /// @returns A strong handle to the asset.
        template <typename T>
        inline Asset store(Asset handle, T data)
        {
            // TODO
            abort();
        }

        /// Gets read-only access to the asset data associated with the given handle.
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called.
        /// @tparam T The type of the asset data.
        /// @param handle The handle to get the asset data for.
        /// @returns A reference to the asset data.
        template <typename T>
        inline const T& get(Asset handle) const
        {
            // TODO
            abort();
        }

        /// Gets read-write access to the asset data associated with the given handle.
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called. This increases the asset's version.
        /// @tparam T The type of the asset data.
        /// @param handle The handle to get the asset data for.
        /// @returns A reference to the asset data.
        template <typename T>
        inline T& get(Asset handle)
        {
            // TODO
            abort();
        }

    private:
        /// Bridges associated to their supported extensions.
        std::unordered_map<std::string, std::unique_ptr<AssetBridge>> bridges;

        /// Metadata for all known assets - loaded or not.
        std::unordered_map<uuids::uuid, AssetMeta> metas;
    };
} // namespace cubos::engine
