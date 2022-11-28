#ifndef CUBOS_ENGINE_DATA_ASSET_HPP
#define CUBOS_ENGINE_DATA_ASSET_HPP

#include <cubos/engine/data/loader.hpp>

#include <cubos/core/data/handle.hpp>
#include <cubos/core/log.hpp>

#include <concepts>
#include <memory>
#include <future>

namespace cubos::engine::data
{
    /// Concept which checks if a type is a valid asset type.
    /// @tparam T The type to check.
    template <typename T>
    concept IsAsset = requires()
    {
        {
            T::TypeName
            } -> std::convertible_to<std::string>;
        requires std::is_base_of_v<Loader, typename T::Loader>;
    };

    /// Handle to an asset with reference counting: it's guaranteed that the asset will be kept alive
    /// as long as the handle is not destroyed.
    /// @tparam T The type of the asset.
    template <typename T>
    requires IsAsset<T>
    class Asset : public core::data::Handle
    {
    public:
        Asset(std::nullptr_t p = nullptr);
        Asset(Asset&&) = default;
        Asset(const Asset&) = default;
        Asset(Handle&& handle);
        ~Asset() = default;

        Asset& operator=(const Asset&) = default;
        const T* operator->() const;

        /// Gets the asset data referenced by this handle.
        /// Will abort if the handle is invalid.
        /// @return The asset data referenced by this handle.
        const T& get() const;

    private:
        friend class AssetManager;

        Asset(size_t* refCount, const T* data, const std::string* id);
    };

    // Implementation.

    template <typename T>
    requires IsAsset<T> Asset<T>::Asset(std::nullptr_t p)
    {
    }

    template <typename T>
    requires IsAsset<T> Asset<T>::Asset(Handle&& handle) : Handle(std::move(handle))
    {
        if (this->type != NULL && this->type != T::TypeName)
        {
            core::logError(
                "Asset(Handle): couldn't get asset handle '{}' of type '{}' from generic handle of type '{}'",
                *this->id, T::TypeName, this->type);

            --(*this->refCount);
            this->refCount = nullptr;
            this->data = nullptr;
            this->type = nullptr;
            this->id = nullptr;
        }
    }

    template <typename T>
    requires IsAsset<T>
    inline const T* Asset<T>::operator->() const
    {
        return &this->get();
    }

    template <typename T>
    requires IsAsset<T>
    const T& Asset<T>::get() const
    {
        if (this->data == nullptr)
        {
            core::logCritical("Asset::get(): can't get reference to data since the handle is null");
            abort();
        }

        return *static_cast<const T*>(this->data);
    }

    template <typename T>
    requires IsAsset<T> Asset<T>::Asset(size_t* refCount, const T* data, const std::string* id)
        : Handle(T::TypeName, refCount, const_cast<void*>(static_cast<const void*>(data)), id)
    {
    }
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_ASSET_HPP
