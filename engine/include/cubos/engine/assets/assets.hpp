#pragma once

#include <condition_variable>
#include <deque>
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>

#include <cubos/core/memory/guards.hpp>
#include <cubos/core/memory/type_map.hpp>

#include <cubos/engine/assets/bridge.hpp>
#include <cubos/engine/assets/meta.hpp>

namespace cubos::engine
{
    /// A read-only guard for an asset's metadata.
    using AssetMetaRead = core::memory::ReadGuard<AssetMeta, std::shared_lock<std::shared_mutex>>;

    /// A read-write guard for an asset's metadata.
    using AssetMetaWrite = core::memory::WriteGuard<AssetMeta, std::unique_lock<std::shared_mutex>>;

    /// A read-only guard for an asset's data.
    /// @tparam T The type of the data to guard.
    template <typename T>
    using AssetRead = core::memory::ReadGuard<T, std::shared_lock<std::shared_mutex>>;

    /// A read-write guard for an asset's data.
    /// @tparam T The type of the data to guard.
    template <typename T>
    using AssetWrite = core::memory::WriteGuard<T, std::unique_lock<std::shared_mutex>>;

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
            Unknown,  ///< No metadata is associated with the asset.
            Unloaded, ///< The asset is not loaded.
            Loading,  ///< The asset is being loaded.
            Loaded,   ///< The asset is loaded.
        };

        Assets();
        Assets(const Assets&) = delete;
        Assets(Assets&&) = delete;
        ~Assets();

        /// Registers a new bridge for the given extension.
        /// @param extension The extension to register the bridge for.
        /// @param bridge The bridge to register.
        void registerBridge(const std::string& extension, std::shared_ptr<AssetBridge> bridge);

        /// Cleans up all assets that are not in use.
        /// Should be called periodically to free up memory.
        void cleanup();

        /// Loads all metadata from the virtual filesystem, in the given path.
        /// If the path is a directory, it will be recursively searched for metadata files.
        /// @param path The path to load metadata from.
        void loadMeta(std::string_view path);

        /// Loads the asset with the given handle, upgrading the handle to a strong one.
        /// This method doesn't block, and the asset may not yet be loaded when it returns.
        /// If the manager is unable to load the asset, a null handle is returned. If an error
        /// occurs while loading the asset, it will only fail in `get` or be visible through
        /// `status`.
        /// @param handle The handle to load the asset for.
        /// @returns A strong handle to the asset, or a null handle if an error occurred.
        AnyAsset load(AnyAsset handle) const;

        /// Saves changes made to an asset's metadata. This function blocks until the asset is
        /// saved.
        /// @param handle The handle identifying the asset to save.
        /// @returns True if the operation successful, false otherwise.
        bool saveMeta(const AnyAsset& handle) const;

        /// Saves changes made to the asset with the given handle. This function blocks until the
        /// asset is saved.
        /// @param handle The handle identifying the asset to save.
        /// @returns True if the operation successful, false otherwise.
        bool save(const AnyAsset& handle) const;

        /// Gets read-only access to the metadata associated with the given handle.
        /// Aborts if the asset is unknown.
        /// @param handle The handle to get the metadata for.
        /// @returns Reference to the metadata.
        AssetMetaRead readMeta(const AnyAsset& handle) const;

        /// Gets read-write access to the metadata associated with the given handle.
        /// If the asset is unknown, an empty metadata object is returned.
        /// @param handle The handle to get the metadata for.
        /// @returns Reference to the metadata.
        AssetMetaWrite writeMeta(const AnyAsset& handle);

        /// Gets read-only access to the asset data associated with the given handle.
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called.
        /// @tparam T The type of the asset data.
        /// @param handle The handle to get the asset data for.
        /// @returns A reference to the asset data.
        template <typename T>
        inline AssetRead<T> read(Asset<T> handle) const
        {
            // Create a strong handle to the asset, so that the asset starts loading if it isn't already.
            auto strong = this->load(handle);
            auto lock = this->lockRead(strong);
            auto data = static_cast<const T*>(this->access(strong, typeid(T), lock, false));
            return AssetRead<T>(*data, std::move(lock));
        }

        /// Gets read-write access to the asset data associated with the given handle.
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called. This increases the asset's version.
        /// @tparam T The type of the asset data.
        /// @param handle The handle to get the asset data for.
        /// @returns A reference to the asset data.
        template <typename T>
        inline AssetWrite<T> write(Asset<T> handle)
        {
            // Create a strong handle to the asset, so that the asset starts loading if it isn't already.
            auto strong = this->load(handle);
            auto lock = this->lockWrite(strong);
            auto data = static_cast<T*>(this->access(strong, typeid(T), lock, true));
            return AssetWrite<T>(*data, std::move(lock));
        }

        /// Checks the status of the asset with the given handle.
        /// @param handle The handle to check the status for.
        /// @returns The status of the asset.
        Status status(const AnyAsset& handle) const;

        /// Updates the given handle to the latest version of the asset.
        /// Can be used to provide asset hot-reloading.
        /// @param handle The handle to update.
        /// @returns Whether the version was updated.
        bool update(AnyAsset& handle);

        /// Forces the given asset to be reloaded.
        /// @param handle The handle to reload.
        void invalidate(const AnyAsset& handle);

        /// Creates a new asset with the given data (and empty metadata) and returns an handle to
        /// it. A random UUID is generated for the asset.
        /// @tparam T The type of the asset data.
        /// @param data The asset data to store.
        /// @returns A strong handle to the newly created asset.
        template <typename T>
        inline Asset<T> create(T data)
        {
            return this->create(typeid(T), new T(std::move(data)), [](void* data) { delete static_cast<T*>(data); });
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
        inline AnyAsset store(AnyAsset handle, T data)
        {
            return this->store(handle, typeid(T), new T(std::move(data)),
                               [](void* data) { delete static_cast<T*>(data); });
        }

    private:
        /// Represents a known asset - may or may not be loaded.
        struct Entry
        {
            Entry();

            Status status{Status::Unloaded}; ///< The status of the asset.
            AssetMeta meta;                  ///< The metadata associated with the asset.

            std::atomic<int> refCount;        ///< Number of strong handles referencing the asset.
            int version{0};                   ///< Number of times the asset has been updated.
            std::shared_mutex mutex;          ///< Mutex for the asset data.
            std::condition_variable_any cond; ///< Triggered when the asset is loaded.

            void* data{nullptr};       ///< Pointer to the asset data, if loaded. Otherwise, nullptr.
            std::type_index type;      ///< The type of the asset data - initially typeid(void).
            void (*destructor)(void*); ///< The destructor for the asset data - initially nullptr.
        };

        /// Stores all data necessary to load an asset.
        struct Task
        {
            AnyAsset handle;                     ///< The handle to load the asset for.
            std::shared_ptr<AssetBridge> bridge; ///< The bridge to use to load the asset.
        };

        /// Untyped version of `create`.
        /// @param type The type of the asset data.
        /// @param data The asset data to store.
        /// @param destructor The destructor for the asset data.
        /// @returns A strong handle to the asset.
        AnyAsset create(std::type_index type, void* data, void (*destructor)(void*));

        /// Untyped version of `store`.
        /// @param handle The handle to associate the asset with.
        /// @param type The type of the asset data.
        /// @param data The asset data to store.
        /// @param destructor The destructor for the asset data.
        /// @returns A strong handle to the asset.
        AnyAsset store(AnyAsset handle, std::type_index type, void* data, void (*destructor)(void*));

        /// Gets a pointer to the asset data associated with the given handle.
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called. If this function is called from the loader thread, instead of waiting
        /// for the asset to load, it will be loaded synchronously.
        /// @tparam Lock The type of the lock guard.
        /// @param handle The handle to get the asset data for.
        /// @param type The expected type of the asset data.
        /// @param lock The lock guard for the asset.
        /// @param incVersion Whether to increase the asset's version.
        /// @returns A pointer to the asset data.
        template <typename Lock>
        void* access(const AnyAsset& handle, std::type_index type, Lock& lock, bool incVersion) const;

        /// Locks the given asset for reading.
        /// @param handle The handle to lock.
        /// @returns The lock guard.
        std::shared_lock<std::shared_mutex> lockRead(const AnyAsset& handle) const;

        /// Locks the given asset for writing.
        /// @param handle The handle to lock.
        /// @returns The lock guard.
        std::unique_lock<std::shared_mutex> lockWrite(const AnyAsset& handle) const;

        /// @param handle The handle to get the entry for.
        /// @returns The entry for the given handle, or nullptr if there is no such entry.
        std::shared_ptr<Entry> entry(const AnyAsset& handle) const;

        /// @param handle The handle to get the entry for.
        /// @param create Whether to create the entry if it doesn't exist.
        /// @returns The entry for the given handle, or nullptr if the handle is invalid.
        std::shared_ptr<Entry> entry(const AnyAsset& handle, bool create);

        /// @param handle The handle to get the bridge for.
        /// @returns The bridge used for the given asset, or nullptr if there's no bridge.
        std::shared_ptr<AssetBridge> bridge(const AnyAsset& handle) const;

        /// Forces the given asset to be reloaded.
        /// @param handle The handle to invalidate.
        /// @param shouldLock Locks the asset if true, otherwise assumes the asset is already locked.
        void invalidate(const AnyAsset& handle, bool shouldLock);

        /// Function run by the loader thread.
        void loader();

        /// Bridges associated to their supported extensions.
        std::unordered_map<std::string, std::shared_ptr<AssetBridge>> mBridges;

        /// Info for all known assets.
        std::unordered_map<uuids::uuid, std::shared_ptr<Entry>> mEntries;

        /// Merseenne Twister used for random UUID generation.
        std::optional<std::mt19937> mRandom;

        /// Read-write lock protecting the bridges and entries maps.
        mutable std::shared_mutex mMutex;

        /// Loader thread for asynchronous loading.
        std::thread mLoaderThread;
        mutable std::deque<Task> mLoaderQueue;       ///< Queued tasks for the loader thread.
        mutable std::mutex mLoaderMutex;             ///< Mutex for the loader queue.
        mutable std::condition_variable mLoaderCond; ///< Triggered on queue change or on exit.
        bool mLoaderShouldExit;                      ///< Whether the loader thread should exit.
    };
} // namespace cubos::engine
