#ifndef CUBOS_ENGINE_DATA_ASSET_MANAGER_HPP
#define CUBOS_ENGINE_DATA_ASSET_MANAGER_HPP

#include <cubos/engine/data/meta.hpp>
#include <cubos/engine/data/asset.hpp>
#include <cubos/engine/data/loader.hpp>

#include <cubos/core/data/file.hpp>

#include <string>
#include <future>
#include <mutex>
#include <map>

namespace cubos::engine::data
{
    /// Manages the loading and unloading of generic assets.
    /// Provides methods to register and unregister asset loaders.
    class AssetManager final
    {
    public:
        AssetManager() = default;
        ~AssetManager();

        /// Loads all meta datas in the given directory or file, recursively.
        /// Meta data files must end with the extension ".meta".
        /// If an already loaded asset is found, abort() is called.
        /// This function isn't thread safe.
        //// @param file The file or directory to load meta data from.
        void importMeta(core::data::File::Handle file);

        /// Loads all static assets.
        void loadStatic();

        /// Cleans up all assets that should be unloaded.
        void cleanup();

        /// Registers a new type of asset. This function isn't thread safe.
        /// @tparam T The type of the asset.
        /// @tparam LArgs The types of the arguments used for the loader.
        /// @param args The arguments passed to the loader's constructor.
        template <typename T, typename... LArgs>
        requires IsAsset<T>
        void registerType(LArgs... args);

        /// Loads the given asset from its ID, synchronously, and returns a handle to it.
        /// @tparam T The type of the asset.
        /// @param id The ID of the asset to load.
        /// @return Handle to the loaded asset, or nullptr if the loading failed.
        template <typename T>
        requires IsAsset<T> Asset<T> load(const std::string& id);

        /// Loads the given asset from its ID, synchronously, and returns a generic handle to it.
        /// @param id The ID of the asset to load.
        /// @return Handle to the loaded asset, or nullptr if the loading failed.
        core::data::Handle loadAny(const std::string& id);

        /// Stores the given asset data in the asset manager, with a certain
        /// ID. If an asset with the same ID already exists, abort() is called.
        /// @tparam T The type of the asset.
        /// @param id The ID of the asset.
        /// @param usage The usage of the asset.
        /// @param data The data of the asset.
        template <typename T>
        requires IsAsset<T> Asset<T> store(const std::string& id, Usage usage, T&& data);

    private :
        /// Stores runtime information about an asset.
        struct Info
        {
            Info(Meta&& meta);
            Info(Meta&& meta, const void* data, std::function<void(const void*)> deleter);
            Info(Info&&) = default;

            Meta meta;                                ///< Asset's meta data.
            const void* data;                         ///< Pointer to the asset's data, if its loaded.
            size_t refCount;                          ///< Number of references to the asset's data.
            std::mutex mutex;                         ///< Protects the asset's state.
            bool stored;                              ///< Was the asset stored, instead of loaded normally?
            std::function<void(const void*)> deleter; ///< Function used to delete the asset, if it was stored.
        };

        std::map<std::string, Loader*> loaders; ///< The loaders of the registered asset types.
        std::map<std::string, Info> infos;      ///< Asset's runtime information.
    };

    // Implementation.

    template <typename T, typename... LArgs>
    requires IsAsset<T>
    void AssetManager::registerType(LArgs... args)
    {
        auto it = this->loaders.find(T::TypeName);
        if (it == this->loaders.end())
        {
            this->loaders[T::TypeName] = new typename T::Loader(this, args...);
        }
        else
        {
            CUBOS_CRITICAL("Could not register asset type because another type with the "
                           "name '{}' is already registered",
                           T::TypeName);
            abort();
        }
    }

    template <typename T>
    requires IsAsset<T> Asset<T> AssetManager::load(const std::string& id)
    {
        return Asset<T>(this->loadAny(id));
    }

    template <typename T>
    requires IsAsset<T> Asset<T> AssetManager::store(const std::string& id, Usage usage, T&& data)
    {
        auto it = this->infos.find(id);
        if (it != this->infos.end())
        {
            CUBOS_CRITICAL("Could not store asset '{}': an asset with this ID already "
                           "exists",
                           id);
            abort();
        }

        this->infos.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                            std::forward_as_tuple(Meta(id, T::TypeName, usage), new T(std::move(data)),
                                                  [](const void* data) { delete static_cast<const T*>(data); }));

        it = this->infos.find(id);
        return Asset<T>(&it->second.refCount, static_cast<const T*>(it->second.data), &it->first);
    }
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_ASSET_MANAGER_HPP
