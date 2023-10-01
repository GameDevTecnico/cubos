/// @file
/// @brief Class @ref cubos::core::ecs::ResourceManager and related types.
/// @ingroup core-ecs-resource

#pragma once

#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// @brief Utility struct used to reference a resource of type @p T for reading.
    /// @tparam T Resource type.
    /// @ingroup core-ecs-resource
    template <typename T>
    class ReadResource
    {
    public:
        /// @brief Move construct.
        /// @param other Other resource to move from.
        ReadResource(ReadResource&& other) noexcept;

        /// @brief Gets the underlying resource reference.
        /// @return Resource reference.
        const T& get() const;

    private:
        friend class ResourceManager;

        /// @brief Constructs.
        /// @param resource Resource to reference.
        /// @param lock Read lock to hold.
        ReadResource(const T& resource, std::shared_lock<std::shared_mutex>&& lock);

        const T& mResource;
        std::shared_lock<std::shared_mutex> mLock;
    };

    /// @brief Utility struct used to reference a resource of type @p T for writing.
    /// @tparam T Resource type.
    /// @ingroup core-ecs-resource
    template <typename T>
    class WriteResource
    {
    public:
        /// @brief Move construct.
        /// @param other Other resource to move from.
        WriteResource(WriteResource&& other) noexcept;

        /// @brief Gets the underlying resource reference.
        /// @return Resource reference.
        T& get() const;

    private:
        friend class ResourceManager;

        /// @brief Constructs.
        /// @param resource Resource to reference.
        /// @param lock Write lock to hold.
        WriteResource(T& resource, std::unique_lock<std::shared_mutex>&& lock);

        T& mResource;
        std::unique_lock<std::shared_mutex> mLock;
    };

    /// @brief Holds and manages resources.
    ///
    /// Used internally by @ref World.
    ///
    /// @ingroup core-ecs-resource
    class ResourceManager final
    {
    public:
        ResourceManager() = default;
        ~ResourceManager();

        /// @brief Registers a new resource type in the resource manager.
        ///
        /// This function is not thread-safe and should be called before any other function.
        ///
        /// @tparam T Resource type.
        /// @tparam TArgs Types of the arguments of the constructor of the resource.
        /// @param args Arguments of the constructor of the resource.
        template <typename T, typename... TArgs>
        void add(TArgs... args);

        /// @brief Locks a resource for reading and returns it.
        /// @tparam T Resource type.
        /// @return Resource lock.
        template <typename T>
        ReadResource<T> read() const;

        /// @brief Locks a resource for writing and returns it.
        /// @tparam T Resource type.
        /// @return Resource lock.
        template <typename T>
        WriteResource<T> write() const;

    private:
        /// @brief Data specific to a resource.
        struct Resource
        {
            mutable std::shared_mutex mutex;      ///< RW lock for the resource.
            void* data;                           ///< Resource resource data.
            std::function<void(void*)> destroyer; ///< Necessary since the type is unknown.

            Resource(void* data, std::function<void(void*)> destroyer);
        };

        /// @brief Associates resource types to their data.
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
