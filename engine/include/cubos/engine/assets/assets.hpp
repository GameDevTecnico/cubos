/// @file
/// @brief Resource @ref cubos::engine::Assets.
/// @ingroup assets-plugin

#pragma once

#include <condition_variable>
#include <deque>
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>

#include <cubos/core/memory/guards.hpp>
#include <cubos/core/memory/type_map.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridge.hpp>
#include <cubos/engine/assets/meta.hpp>

namespace cubos::engine
{
    /// @brief Read-only guard for an asset's metadata.
    /// @ingroup assets-plugin
    using AssetMetaRead = core::memory::ReadGuard<AssetMeta, std::shared_lock<std::shared_mutex>>;

    /// @brief Read-write guard for an asset's metadata.
    /// @ingroup assets-plugin
    using AssetMetaWrite = core::memory::WriteGuard<AssetMeta, std::unique_lock<std::shared_mutex>>;

    /// @brief Read-only guard for an asset's data.
    /// @tparam T The type of the data to guard.
    /// @ingroup assets-plugin
    template <typename T>
    using AssetRead = core::memory::ReadGuard<T, std::shared_lock<std::shared_mutex>>;

    /// @brief Read-write guard for an asset's data.
    /// @tparam T The type of the data to guard.
    /// @ingroup assets-plugin
    template <typename T>
    using AssetWrite = core::memory::WriteGuard<T, std::unique_lock<std::shared_mutex>>;

    /// @brief Resource which manages all assets. Responsible for loading and unloading assets,
    /// storing them in memory, and providing access to them.
    ///
    /// Assets are all identified through @ref Asset handles.
    /// @ingroup assets-plugin
    class Assets final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Possible statuses for an asset.
        enum class Status
        {
            Unknown,  ///< No metadata is associated with the asset.
            Unloaded, ///< The asset is not loaded.
            Loading,  ///< The asset is being loaded.
            Loaded,   ///< The asset is loaded.
        };

        ~Assets();

        /// @brief Constructs an empty manager without any bridges or metadata.
        Assets();

        /// @name Forbid any kind of copying.
        /// @{
        Assets(const Assets&) = delete;
        Assets& operator=(const Assets&) = delete;
        /// @}

        /// @brief Registers a new bridge for the given extension.
        ///
        /// If more than one extension match a given asset's name, the longest extension is picked.
        ///
        /// @param extension Extension to register the bridge for.
        /// @param bridge Bridge to register.
        void registerBridge(const std::string& extension, std::shared_ptr<AssetBridge> bridge);

        /// @brief Cleans up all assets that are not in use. Should be called periodically to free
        /// up memory.
        void cleanup();

        /// @brief Loads all metadata from the virtual filesystem, in the given path. If the path
        /// points to a directory, it will be recursively searched for metadata files.
        /// @param path Path to load metadata from.
        void loadMeta(std::string_view path);

        /// @brief Loads the asset with the given handle, upgrading the handle to a strong one.
        ///
        /// This method doesn't block, thus the asset may have not yet been loaded when it returns.
        /// If the manager is unable to find the asset or a bridge for loading it, a null handle
        /// is returned. If an error occurs while loading the asset, it will only fail in @ref
        /// read() or be visible through @ref status().
        ///
        /// @param handle Handle to load the asset for.
        /// @return Strong handle to the asset, or a null handle if an error occurred.
        AnyAsset load(AnyAsset handle) const;

        /// @brief Saves changes made to an asset's metadata.
        ///
        /// This method blocks until the asset is saved.
        ///
        /// @param handle Handle identifying the asset to save.
        /// @return Whether the operation was successful.
        bool saveMeta(const AnyAsset& handle) const;

        /// @brief Saves changes made to the asset with the given handle.
        ///
        /// This method blocks until the asset is saved.
        ///
        /// @param handle Handle identifying the asset to save.
        /// @return Whether the operation was successful.
        bool save(const AnyAsset& handle) const;

        /// @brief Gets read-only access to the metadata associated with the given handle.
        ///
        /// Aborts if the asset is unknown.
        ///
        /// @param handle Handle of the asset to get the metadata for.
        /// @return Reference to the metadata.
        AssetMetaRead readMeta(const AnyAsset& handle) const;

        /// @brief Gets read-write access to the metadata associated with the given handle.
        ///
        /// If the asset is unknown, an empty metadata object is returned.
        ///
        /// @param handle Handle to get the metadata for.
        /// @return Reference to the metadata.
        AssetMetaWrite writeMeta(const AnyAsset& handle);

        /// @brief Gets read-only access to the asset data associated with the given handle.
        ///
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called.
        ///
        /// @tparam T Type of the asset data.
        /// @param handle Handle to get the asset data for.
        /// @return Reference to the asset data.
        template <typename T>
        inline AssetRead<T> read(Asset<T> handle) const
        {
            // Create a strong handle to the asset, so that the asset starts loading if it isn't already.
            auto strong = this->load(handle);
            auto lock = this->lockRead(strong);
            auto data = static_cast<const T*>(this->access(strong, core::reflection::reflect<T>(), lock, false));
            return AssetRead<T>(*data, std::move(lock));
        }

        /// @brief Gets read-write access to the asset data associated with the given handle.
        ///
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called. This increases the asset's version.
        ///
        /// @tparam T Type of the asset data.
        /// @param handle Handle to get the asset data for.
        /// @return Reference to the asset data.
        template <typename T>
        inline AssetWrite<T> write(Asset<T> handle)
        {
            // Create a strong handle to the asset, so that the asset starts loading if it isn't already.
            auto strong = this->load(handle);
            auto lock = this->lockWrite(strong);
            auto data = static_cast<T*>(this->access(strong, core::reflection::reflect<T>(), lock, true));
            return AssetWrite<T>(*data, std::move(lock));
        }

        /// @brief Gets the status of the asset with the given handle.
        /// @param handle Handle to check the status for.
        /// @return Status of the asset.
        Status status(const AnyAsset& handle) const;

        /// @brief Updates the given handle to the latest version of the asset.
        ///
        /// Can be used to implement hot-reloading.
        ///
        /// @param handle Handle to update.
        /// @return Whether the version was updated.
        bool update(AnyAsset& handle) const;

        /// @brief Updates the given handle to the latest version of the asset.
        ///
        /// Can be used to implement hot-reloading.
        ///
        /// @param handle Handle to update.
        /// @return Whether the version was updated.
        template <typename T>
        bool update(Asset<T>& handle) const
        {
            return update(static_cast<AnyAsset&>(handle));
        }

        /// @brief Unloads the given asset. Can be used to force assets to be reloaded.
        /// @param handle Handle to unload.
        void invalidate(const AnyAsset& handle);

        /// @brief Creates a new asset with a random UUID with the given data (and empty metadata).
        /// @tparam T Type of the asset data.
        /// @param data Asset data to store.
        /// @return Strong handle to the new asset.
        template <typename T>
        inline Asset<T> create(T data)
        {
            return this->create(core::reflection::reflect<T>(), new T(std::move(data)),
                                [](void* data) { delete static_cast<T*>(data); });
        }

        /// @brief Stores the given asset data in memory, associated with the given handle.
        ///
        /// If an asset with the same handle already exists, it will be replaced.
        /// If no metadata is associated with the handle, an empty one will be created.
        /// This increases the asset's version.
        ///
        /// @tparam T Type of the asset data.
        /// @param handle Handle to associate the asset with.
        /// @param data Asset data to store.
        /// @return Strong handle to the asset.
        template <typename T>
        inline AnyAsset store(AnyAsset handle, T data)
        {
            return this->store(handle, core::reflection::reflect<T>(), new T(std::move(data)),
                               [](void* data) { delete static_cast<T*>(data); });
        }

        /// @brief Gets all assets that have been registered
        /// @return Vector with all registered assets.
        std::vector<AnyAsset> listAll() const;

        /// @brief Gets the type of an asset.
        ///
        /// If the asset is not loaded, its type is deduced from its bridge.
        /// If there's also no associated bridge, aborts.
        /// If the asset does not exist, aborts.
        ///
        /// @param handle Handle to check the type for.
        /// @return Asset type.
        const core::reflection::Type& type(const AnyAsset& handle) const;

    private:
        /// @brief Represents a known asset - may or may not be loaded.
        struct Entry
        {
            Entry();

            Status status{Status::Unloaded}; ///< The status of the asset.
            AssetMeta meta;                  ///< The metadata associated with the asset.

            std::atomic<int> refCount;        ///< Number of strong handles referencing the asset.
            int version{0};                   ///< Number of times the asset has been updated.
            std::shared_mutex mutex;          ///< Mutex for the asset data.
            std::condition_variable_any cond; ///< Triggered when the asset is loaded.

            void* data{nullptr};                         ///< Pointer to the asset data, if loaded. Otherwise, nullptr.
            const core::reflection::Type* type{nullptr}; ///< Type of the asset data.
            void (*destructor)(void*);                   ///< Destructor for the asset data - initially nullptr.
        };

        /// @brief Stores all data necessary to load an asset.
        struct Task
        {
            AnyAsset handle;                     ///< The handle to load the asset for.
            std::shared_ptr<AssetBridge> bridge; ///< The bridge to use to load the asset.
        };

        /// @brief Untyped version of @ref create().
        /// @param type Type of the asset data.
        /// @param data Asset data to store.
        /// @param destructor Destructor for the asset data.
        /// @return Strong handle to the asset.
        AnyAsset create(const core::reflection::Type& type, void* data, void (*destructor)(void*));

        /// @brief Untyped version of @ref store().
        /// @param handle Handle to associate the asset with.
        /// @param type Type of the asset data.
        /// @param data Asset data to store.
        /// @param destructor Destructor for the asset data.
        /// @return Strong handle to the asset.
        AnyAsset store(AnyAsset handle, const core::reflection::Type& type, void* data, void (*destructor)(void*));

        /// @brief Gets a pointer to the asset data associated with the given handle.
        ///
        /// If the asset is not loaded, this blocks until it is. If the asset cannot be loaded,
        /// abort is called. If this function is called from the loader thread, instead of waiting
        /// for the asset to load, it will be loaded synchronously.
        ///
        /// @tparam Lock The type of the lock guard.
        /// @param handle Handle to get the asset data for.
        /// @param type Expected type of the asset data.
        /// @param lock Lock guard for the asset.
        /// @param incVersion Whether to increase the asset's version.
        /// @return Pointer to the asset's data.
        template <typename Lock>
        void* access(const AnyAsset& handle, const core::reflection::Type& type, Lock& lock, bool incVersion) const;

        /// @brief Locks the given asset for reading.
        /// @param handle Handle to lock.
        /// @return Lock guard.
        std::shared_lock<std::shared_mutex> lockRead(const AnyAsset& handle) const;

        /// @brief Locks the given asset for writing.
        /// @param handle Handle to lock.
        /// @return Lock guard.
        std::unique_lock<std::shared_mutex> lockWrite(const AnyAsset& handle) const;

        /// @brief Gets a pointer to the entry associated with the given handle.
        /// @param handle Handle to get the entry for.
        /// @return Entry for the given handle, or nullptr if there is no such entry.
        std::shared_ptr<Entry> entry(const AnyAsset& handle) const;

        /// @brief Gets a pointer to the entry associated with the given handle.
        /// @param handle Handle to get the entry for.
        /// @param create Whether to create the entry if it doesn't exist.
        /// @return Entry for the given handle, or nullptr if the handle is invalid.
        std::shared_ptr<Entry> entry(const AnyAsset& handle, bool create);

        /// @brief Gets a pointer to the bridge to be used for loading the asset identified by the
        /// given handle.
        /// @param handle Handle to get the bridge for.
        /// @return Bridge used for the given asset, or nullptr if there's no bridge.
        std::shared_ptr<AssetBridge> bridge(const AnyAsset& handle) const;

        /// @brief Unloads the given asset. Can be used to force assets to be reloaded.
        /// @param handle Handle to unload.
        /// @param shouldLock Locks the asset if true, otherwise assumes the asset is already locked.
        void invalidate(const AnyAsset& handle, bool shouldLock);

        /// @brief Function run by the loader thread.
        void loader();

        /// @brief Bridges associated to their supported extensions.
        std::unordered_map<std::string, std::shared_ptr<AssetBridge>> mBridges;

        /// @brief Info for all known assets.
        std::unordered_map<uuids::uuid, std::shared_ptr<Entry>> mEntries;

        /// @brief Mersenne Twister used for random UUID generation.
        std::optional<std::mt19937> mRandom;

        /// @brief Read-write lock protecting the bridges and entries maps.
        mutable std::shared_mutex mMutex;

        /// @brief Loader thread for asynchronous loading.
        std::thread mLoaderThread;
        mutable std::deque<Task> mLoaderQueue;       ///< Queued tasks for the loader thread.
        mutable std::mutex mLoaderMutex;             ///< Mutex for the loader queue.
        mutable std::condition_variable mLoaderCond; ///< Triggered on queue change or on exit.
        bool mLoaderShouldExit;                      ///< Whether the loader thread should exit.
    };
} // namespace cubos::engine
