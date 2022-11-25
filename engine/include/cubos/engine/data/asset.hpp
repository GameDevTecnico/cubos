#ifndef CUBOS_ENGINE_DATA_ASSET_HPP
#define CUBOS_ENGINE_DATA_ASSET_HPP

#include <cubos/engine/data/loader.hpp>
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
    /// as long as the handle is not destroyed. Can be cast to a weak asset handle, but not the other
    /// way around.
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

        operator bool() const;
        const T* operator->() const;
        Asset& operator=(const Asset& rhs);

        /// Checks if the handle references a valid asset.
        /// @return True if the handle references a valid asset, false otherwise.
        bool isValid() const;

        /// Gets the asset data referenced by this handle.
        /// Will abort if the handle is invalid.
        /// @return The asset data referenced by this handle.
        const T& get() const;

        /// Gets the id of the asset referenced by this handle.
        /// Will abort if the handle is invalid.
        /// @return The id of the asset referenced by this handle.
        const std::string& getId() const;

    private:
        friend class AssetManager;

        /// Creates a new handle.
        /// @param refCount Pointer to the reference counter.
        /// @param asset The asset to reference.
        /// @param id The id of the asset.
        Asset(size_t* refCount, const T* ptr, const std::string* id);

        size_t* refCount;      ///< Pointer to the reference counter.
        const T* ptr;          ///< Pointer to the asset.
        const std::string* id; ///< Pointer to the id of the asset.
    };

    /// Weak handle to an asset: doesn't own the asset, and you can't read the asset data from it.
    /// It must be first converted to a strong handle to be used, through the AssetManager::load()
    /// method.
    /// @tparam T The type of the asset.
    template <typename T>
    requires IsAsset<T>
    class WeakAsset
    {
    public:
        WeakAsset(const std::string& id = "");
        WeakAsset(WeakAsset&&) = default;
        WeakAsset(const WeakAsset&) = default;
        WeakAsset(const Asset<T>&);
        ~WeakAsset() = default;

        WeakAsset& operator=(const WeakAsset& rhs);
        WeakAsset& operator=(const Asset<T>& rhs);

        /// Gets the id of the asset referenced by this handle.
        /// @return The id of the asset referenced by this handle.
        const std::string& getId() const;

        void serialize(core::data::Serializer& serializer, const char* name) const;
        void deserialize(core::data::Deserializer& deserializer);

    private:
        std::string id;
    };

    // Implementation.

    template <typename T>
    requires IsAsset<T> Asset<T>::Asset(std::nullptr_t)
    {
        this->refCount = nullptr;
        this->ptr = nullptr;
        this->id = nullptr;
    }

    template <typename T>
    requires IsAsset<T> Asset<T>::Asset(Asset&& rhs)
    {
        this->refCount = rhs.refCount;
        this->ptr = rhs.ptr;
        this->id = rhs.id;
        rhs.refCount = nullptr;
        rhs.ptr = nullptr;
        rhs.id = nullptr;
    }

    template <typename T>
    requires IsAsset<T> Asset<T>::Asset(const Asset& rhs)
    {
        this->refCount = rhs.refCount;
        this->ptr = rhs.ptr;
        this->id = rhs.id;

        if (this->ptr != nullptr)
        {
            ++(*this->refCount);
        }
    }

    template <typename T>
    requires IsAsset<T> Asset<T>::Asset(size_t* refCount, const T* ptr, const std::string* id)
    {
        this->refCount = refCount;
        this->ptr = ptr;
        this->id = id;

        if (this->ptr != nullptr)
        {
            ++(*this->refCount);
        }
    }

    template <typename T>
    requires IsAsset<T> Asset<T>::~Asset()
    {
        if (this->ptr != nullptr)
        {
            --(*this->refCount);
        }
    }

    template <typename T>
    requires IsAsset<T>
    inline Asset<T>::operator bool() const
    {
        return this->isValid();
    }

    template <typename T>
    requires IsAsset<T>
    inline const T* Asset<T>::operator->() const
    {
        return &this->get();
    }

    template <typename T>
    requires IsAsset<T>
    inline Asset<T>& Asset<T>::operator=(const Asset& rhs)
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

    template <typename T>
    requires IsAsset<T>
    const T& Asset<T>::get() const
    {
        if (this->ptr == nullptr)
        {
            core::logCritical("Asset::get(): can't get reference to data since the handle is null");
            abort();
        }
        return *this->ptr;
    }

    template <typename T>
    requires IsAsset<T>
    const std::string& Asset<T>::getId() const
    {
        if (this->id == nullptr)
        {
            core::logCritical("Asset::getId(): can't get id since the handle is null");
            abort();
        }
        return *this->id;
    }

    template <typename T>
    requires IsAsset<T>
    bool Asset<T>::isValid() const
    {
        return this->ptr != nullptr;
    }

    template <typename T>
    requires IsAsset<T> WeakAsset<T>::WeakAsset(const std::string& id)
    {
        this->id = id;
    }

    template <typename T>
    requires IsAsset<T> WeakAsset<T>::WeakAsset(const Asset<T>& rhs)
    {
        this->id = rhs.getId();
    }

    template <typename T>
    requires IsAsset<T> WeakAsset<T>
    &WeakAsset<T>::operator=(const WeakAsset& rhs)
    {
        this->id = rhs.id;
        return *this;
    }

    template <typename T>
    requires IsAsset<T> WeakAsset<T>
    &WeakAsset<T>::operator=(const Asset<T>& rhs)
    {
        this->id = rhs.getId();
        return *this;
    }

    template <typename T>
    requires IsAsset<T>
    const std::string& WeakAsset<T>::getId() const
    {
        return this->id;
    }

    template <typename T>
    requires IsAsset<T>
    void WeakAsset<T>::serialize(core::data::Serializer& serializer, const char* name) const
    {
        serializer.write(this->id, name);
    }

    template <typename T>
    requires IsAsset<T>
    void WeakAsset<T>::deserialize(core::data::Deserializer& deserializer)
    {
        deserializer.read(this->id);
    }
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_ASSET_HPP
