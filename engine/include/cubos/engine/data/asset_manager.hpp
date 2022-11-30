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

        /// Loads the given asset from its ID, synchronously.
        /// @tparam T The type of the asset.
        /// @param id The ID of the asset to load.
        /// @return Handle to the loaded asset.
        template <typename T>
        requires IsAsset<T> Asset<T> load(const std::string& id);

        /// Stores the given asset data in the asset manager, with a certain
        /// ID. If an asset with the same ID is already stored, it is replaced.
        /// Assets created through this function are always dynamic, and should
        /// @tparam T The type of the asset.

    private :
        /// Stores runtime information about an asset.
        struct Info
        {
            Info(Meta&& meta);

            Meta meta;        ///< Asset's meta data.
            const void* data; ///< Pointer to the asset's data, if its loaded.
            size_t refCount;  ///< Number of references to the asset's data.
            std::mutex mutex; ///< Protects the asset's state.
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
            core::logError("AssetManager::registerType(): couldn't register asset type because another type with the "
                           "name '{}' was already register",
                           T::TypeName);
            abort();
        }
    }

    template <typename T>
    requires IsAsset<T> Asset<T> AssetManager::load(const std::string& id)
    {
        auto it = this->infos.find(id);
        if (it == this->infos.end())
        {
            core::logError("AssetManager::load(): couldn't load asset '{}' because it wasn't found", id);
            return nullptr;
        }

        if (it->second.meta.getType() != T::TypeName)
        {
            core::logError("AssetManager::load(): couldn't load asset '{}' because it's type is '{}' but the type "
                           "expected is '{}'",
                           id, it->second.meta.getType(), T::TypeName);
            return nullptr;
        }

        // Check if it's loaded and load it if it isn't.
        std::lock_guard lock(it->second.mutex);

        if (it->second.data == nullptr)
        {
            auto lit = this->loaders.find(T::TypeName);
            if (lit == this->loaders.end())
            {
                core::logError("AssetManager::load(): couldn't load asset '{}' because the loader for type '{}' wasn't "
                               "found",
                               id, T::TypeName);
                abort();
            }

            it->second.data = lit->second->load(it->second.meta);
            if (it->second.data == nullptr)
            {
                core::logError("AssetManager::load(): couldn't load '{}'", it->second.meta.getId());
                return nullptr;
            }
            else
            {
                core::logInfo("AssetManager::load(): loaded '{}'", it->second.meta.getId());
            }
        }

        return Asset<T>(&it->second.refCount, static_cast<const T*>(it->second.data));
    }

} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_ASSET_MANAGER_HPP
