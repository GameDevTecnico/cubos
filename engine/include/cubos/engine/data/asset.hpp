#ifndef CUBOS_ENGINE_DATA_ASSET_HPP
#define CUBOS_ENGINE_DATA_ASSET_HPP

#include <cubos/engine/data/loader.hpp>

#include <cubos/core/log.hpp>

#include <concepts>
#include <memory>

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

    /// Wrapper class that stores a reference to an asset, keeping track of how many references it has.
    /// @tparam T The type of the asset.
    template <typename T>
    requires IsAsset<T>
    class Asset
    {
    public:
        Asset(std::nullptr_t x = nullptr);
        Asset(Asset&&);
        Asset(const Asset&);
        ~Asset();

        const T& get() const;
        const T* operator->() const;
        operator bool() const;

        Asset& operator=(const Asset& rhs);

    private:
        friend class AssetManager;

        /// Creates a new handle.
        /// @param refCount Pointer to the reference counter.
        /// @param asset The asset to reference.
        Asset(size_t* refCount, const T* ptr);

        size_t* refCount; ///< Pointer to the reference counter.
        const T* ptr;     ///< Pointer to the asset.
    };

    // Implementation.

    template <typename T> Asset<T>::Asset(std::nullptr_t)
    {
        this->refCount = nullptr;
        this->ptr = nullptr;
    }

    template <typename T> Asset<T>::Asset(Asset&& rhs)
    {
        this->refCount = rhs.refCount;
        this->ptr = rhs.ptr;
        rhs.refCount = nullptr;
        rhs.ptr = nullptr;
    }

    template <typename T> Asset<T>::Asset(const Asset& rhs)
    {
        this->refCount = rhs.refCount;
        this->ptr = rhs.ptr;

        if (this->ptr != nullptr)
        {
            ++(*this->refCount);
        }
    }

    template <typename T> Asset<T>::Asset(size_t* refCount, const T* ptr)
    {
        this->refCount = refCount;
        this->ptr = ptr;

        if (this->ptr != nullptr)
        {
            ++(*this->refCount);
        }
    }

    template <typename T> Asset<T>::~Asset()
    {
        if (this->ptr != nullptr)
        {
            --(*this->refCount);
        }
    }

    template <typename T> const T& Asset<T>::get() const
    {
        if (this->ptr == nullptr)
        {
            core::logError("Asset::get(): can't get reference to data since the handle is null");
            abort();
        }
        return *this->ptr;
    }

    template <typename T> inline const T* Asset<T>::operator->() const
    {
        return &this->get();
    }

    template <typename T> inline Asset<T>::operator bool() const
    {
        return this->ptr != nullptr;
    }

    template <typename T> inline Asset<T>& Asset<T>::operator=(const Asset& rhs)
    {
        if (this->ptr != rhs.ptr)
        {
            if (this->ptr != nullptr)
            {
                --(*this->refCount);
            }

            this->refCount = rhs.refCount;
            this->ptr = rhs.ptr;

            if (this->ptr != nullptr)
            {
                ++(*this->refCount);
            }
        }

        return *this;
    }

} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_ASSET_HPP
