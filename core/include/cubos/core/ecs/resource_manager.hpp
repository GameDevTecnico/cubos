#pragma once

#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// Utility struct used to reference resources for reading.
    template <typename T>
    class ReadResource
    {
    public:
        ReadResource(ReadResource&& /*other*/) noexcept;

        /// Gets the underlying resource reference.
        const T& get() const;

    private:
        friend class ResourceManager;

        ReadResource(const T& resource, std::shared_lock<std::shared_mutex>&& lock);

        const T& mResource;
        std::shared_lock<std::shared_mutex> mLock;
    };

    /// Utility struct used to reference resources for writing.
    template <typename T>
    class WriteResource
    {
    public:
        WriteResource(WriteResource&& /*other*/) noexcept;

        /// Gets the underlying resource reference.
        T& get() const;

    private:
        friend class ResourceManager;

        WriteResource(T& resource, std::unique_lock<std::shared_mutex>&& lock);

        T& mResource;
        std::unique_lock<std::shared_mutex> mLock;
    };

    /// Object which manages the resources of the ECS world.
    /// Used internally by the ECS world.
    class ResourceManager final
    {
    public:
        ResourceManager() = default;
        ~ResourceManager();

        /// Registers a new resource type in the resource manager.
        /// Unsafe to call during any reads or writes, should be called at the start of the program.
        /// @tparam T The type of the resource.
        /// @tparam TArgs The types of the arguments of the constructor of the resource.
        /// @param args The arguments of the constructor of the resource.
        template <typename T, typename... TArgs>
        void add(TArgs... args);

        /// Reads a resource from the resource manager, locking it for reading.
        /// @tparam T The type of the resource.
        /// @returns A lock referring to the resource.
        template <typename T>
        ReadResource<T> read() const;

        /// Writes a resource to the resource manager, locking it for writing.
        /// @tparam T The type of the resource.
        /// @returns A lock referring to the resource.
        template <typename T>
        WriteResource<T> write() const;

    private:
        /// Data specific to a resource.
        struct Resource
        {
            mutable std::shared_mutex mutex;      ///> RW lock for the resource.
            void* data;                           ///< The data of the resource.
            std::function<void(void*)> destroyer; ///< Necessary since the type is unknown.

            Resource(void* data, std::function<void(void*)> destroyer);
        };

        /// Associates resource types to their data.
        std::unordered_map<std::type_index, Resource> mResources;
    };

    // Implementation.

    template <typename T>
    ReadResource<T>::ReadResource(ReadResource&& other) noexcept
        : mResource(other.mResource)
        , mLock(std::move(other.mLock))
    {
        // Do nothing.
    }

    template <typename T>
    ReadResource<T>::ReadResource(const T& resource, std::shared_lock<std::shared_mutex>&& lock)
        : mResource(resource)
        , mLock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T>
    const T& ReadResource<T>::get() const
    {
        return mResource;
    }

    template <typename T>
    WriteResource<T>::WriteResource(WriteResource&& other) noexcept
        : mResource(other.mResource)
        , mLock(std::move(other.mLock))
    {
        // Do nothing.
    }

    template <typename T>
    WriteResource<T>::WriteResource(T& resource, std::unique_lock<std::shared_mutex>&& lock)
        : mResource(resource)
        , mLock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T>
    T& WriteResource<T>::get() const
    {
        return mResource;
    }

    inline ResourceManager::~ResourceManager()
    {
        for (auto& [type, resource] : mResources)
        {
            resource.destroyer(resource.data);
        }
    }

    inline ResourceManager::Resource::Resource(void* data, std::function<void(void*)> destroyer)
        : data(data)
        , destroyer(std::move(std::move(destroyer)))
    {
        // Do nothing.
    }

    template <typename T, typename... TArgs>
    void ResourceManager::add(TArgs... args)
    {
        auto it = mResources.find(typeid(T));
        if (it != mResources.end())
        {
            CUBOS_CRITICAL("Could not register resource of type {}: already registered", typeid(T).name());
            abort();
        }

        mResources.try_emplace(typeid(T), new T(args...), [](void* data) { delete static_cast<T*>(data); });
    }

    template <typename T>
    ReadResource<T> ResourceManager::read() const
    {
        auto it = mResources.find(typeid(T));
        if (it == mResources.end())
        {
            CUBOS_CRITICAL("Could not find resource of type {}", typeid(T).name());
            abort();
        }

        const auto& resource = it->second;
        return ReadResource<T>(*static_cast<const T*>(resource.data), std::shared_lock(resource.mutex));
    }

    template <typename T>
    WriteResource<T> ResourceManager::write() const
    {
        auto it = mResources.find(typeid(T));
        if (it == mResources.end())
        {
            CUBOS_CRITICAL("Could not find resource of type {}", typeid(T).name());
            abort();
        }

        const auto& resource = it->second;
        return WriteResource<T>(*static_cast<T*>(resource.data), std::unique_lock(resource.mutex));
    }
} // namespace cubos::core::ecs
