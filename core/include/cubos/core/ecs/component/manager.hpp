/// @file
/// @brief Class @ref cubos::core::ecs::ComponentManager and related types.
/// @ingroup core-ecs-component

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <typeindex>

#include <cubos/core/ecs/component/storage.hpp>

namespace cubos::core::ecs
{
    /// @brief Gets the registered name of a component type.
    ///
    /// If the component type is not registered, aborts the program.
    ///
    /// @tparam T Component type.
    /// @return Registered name of the component type.
    /// @ingroup core-ecs-component
    template <typename T>
    std::optional<std::string_view> getComponentName();

    /// @brief Gets the registered name of a component type.
    ///
    /// If the component type is not registered, aborts the program.
    ///
    /// @param type Component type.
    /// @return Registered name of the component type.
    /// @ingroup core-ecs-component
    std::optional<std::string_view> getComponentName(std::type_index type);

    /// @brief Utility struct used to reference a storage of component type @p T for reading.
    /// @tparam T Component type.
    /// @ingroup core-ecs-component
    template <typename T>
    class ReadStorage
    {
    public:
        /// @brief Move constructor.
        /// @param other Other instance to move from.
        ReadStorage(ReadStorage&& other) noexcept;

        /// @brief Gets the underlying storage reference.
        /// @return Underlying storage reference.
        const Storage<T>& get() const;

    private:
        friend class ComponentManager;

        /// @brief Constructs.
        /// @param storage  Storage to reference.
        /// @param lock Read lock to hold.
        ReadStorage(const Storage<T>& storage, std::shared_lock<std::shared_mutex>&& lock);

        const Storage<T>& mStorage;
        std::shared_lock<std::shared_mutex> mLock;
    };

    /// @brief Utility struct used to reference a storage of component type @p T for writing.
    /// @tparam T Component type.
    /// @ingroup core-ecs-component
    template <typename T>
    class WriteStorage
    {
    public:
        /// @brief Move constructor.
        /// @param other Other instance to move from.
        WriteStorage(WriteStorage&& other) noexcept;

        /// @brief Gets the underlying storage reference.
        /// @return Underlying storage reference.
        Storage<T>& get() const;

    private:
        friend class ComponentManager;

        /// @brief Constructs.
        /// @param storage Storage to reference.
        /// @param lock Write lock to hold.
        WriteStorage(Storage<T>& storage, std::unique_lock<std::shared_mutex>&& lock);

        Storage<T>& mStorage;
        std::unique_lock<std::shared_mutex> mLock;
    };

    /// @brief Holds and manages components.
    ///
    /// Used internally by @ref World.
    ///
    /// @ingroup core-ecs-component
    class ComponentManager final
    {
    public:
        /// @brief Registers a new component type with the component manager.
        ///
        /// Must be called before any component of this type is used in any way.
        ///
        /// @tparam T Type of the component.
        template <typename T>
        void registerComponent();

        /// @brief Registers a new component type with the component manager.
        ///
        /// Must be called before any component of this type is used in any way.
        ///
        /// @param type Type of the component.
        void registerComponent(std::type_index type);

        /// @brief Gets the identifier of a registered component type.
        /// @param type Component type.
        /// @return Component identifier.
        std::size_t getIDFromIndex(std::type_index type) const;

        /// @brief Gets the identifier of a registered component type.
        /// @tparam T Component type.
        /// @return Component identifier.
        template <typename T>
        std::size_t getID() const;

        /// @brief Gets the type of a component from its identifier.
        /// @param id Component identifier.
        /// @return Component type index.
        std::type_index getType(std::size_t id) const;

        //// @brief Locks a storage for reading and returns it.
        /// @tparam T Component type.
        /// @return Storage lock.
        template <typename T>
        ReadStorage<T> read() const;

        //// @brief Locks a storage for writing and returns it.
        /// @tparam T Component type.
        /// @return Storage lock.
        template <typename T>
        WriteStorage<T> write() const;

        /// @brief Adds a component to an entity.
        /// @tparam T Component type.
        /// @param id Entity index.
        /// @param value Initial component value.
        template <typename T>
        void add(uint32_t id, T value);

        /// @brief Removes a component from an entity.
        /// @tparam T Component type.
        /// @param id Entity index.
        template <typename T>
        void remove(uint32_t id);

        /// @brief Removes a component from an entity.
        /// @param id Entity index.
        /// @param componentId Component identifier.
        void remove(uint32_t id, std::size_t componentId);

        /// @brief Removes all components from an entity.
        /// @param id Entity index.
        void removeAll(uint32_t id);

        /// @brief Creates a package from a component of an entity.
        /// @param id Entity index.
        /// @param componentId Component identifier.
        /// @param context Optional context to use for serialization.
        /// @return Package containing the component.
        data::old::Package pack(uint32_t id, std::size_t componentId, data::old::Context* context) const;

        /// @brief Inserts a component into an entity, by unpacking a package.
        /// @param id Entity index.
        /// @param componentId Component identifier.
        /// @param package Package to unpack.
        /// @param context Optional context to use for deserialization.
        /// @return Whether the unpacking was successful.
        bool unpack(uint32_t id, std::size_t componentId, const data::old::Package& package,
                    data::old::Context* context);

    private:
        struct Entry
        {
            Entry(std::unique_ptr<IStorage> storage);

            std::unique_ptr<IStorage> storage;        ///< Generic component storage.
            std::unique_ptr<std::shared_mutex> mutex; ///< Read/write lock for the storage.
        };

        /// @brief Maps component types to component IDs.
        std::unordered_map<std::type_index, std::size_t> mTypeToIds;

        std::vector<Entry> mEntries; ///< Registered component storages.
    };

    // Implementation.

    template <typename T>
    std::optional<std::string_view> getComponentName()
    {
        return getComponentName(typeid(T));
    }

    template <typename T>
    ReadStorage<T>::ReadStorage(ReadStorage&& other) noexcept
        : mStorage(other.mStorage)
        , mLock(std::move(other.mLock))
    {
        // Do nothing.
    }

    template <typename T>
    const Storage<T>& ReadStorage<T>::get() const
    {
        return mStorage;
    }

    template <typename T>
    ReadStorage<T>::ReadStorage(const Storage<T>& storage, std::shared_lock<std::shared_mutex>&& lock)
        : mStorage(storage)
        , mLock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T>
    WriteStorage<T>::WriteStorage(WriteStorage&& other) noexcept
        : mStorage(other.mStorage)
        , mLock(std::move(other.mLock))
    {
        // Do nothing.
    }

    template <typename T>
    Storage<T>& WriteStorage<T>::get() const
    {
        return mStorage;
    }

    template <typename T>
    WriteStorage<T>::WriteStorage(Storage<T>& storage, std::unique_lock<std::shared_mutex>&& lock)
        : mStorage(storage)
        , mLock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T>
    void ComponentManager::registerComponent()
    {
        this->registerComponent(typeid(T));
    }

    template <typename T>
    std::size_t ComponentManager::getID() const
    {
        return this->getIDFromIndex(typeid(T));
    }

    template <typename T>
    ReadStorage<T> ComponentManager::read() const
    {
        const std::size_t componentId = this->getID<T>();
        return ReadStorage<T>(*static_cast<const Storage<T>*>(mEntries[componentId - 1].storage.get()),
                              std::shared_lock<std::shared_mutex>(*mEntries[componentId - 1].mutex));
    }

    template <typename T>
    WriteStorage<T> ComponentManager::write() const
    {
        const std::size_t componentId = this->getID<T>();
        return WriteStorage<T>(*static_cast<Storage<T>*>(mEntries[componentId - 1].storage.get()),
                               std::unique_lock<std::shared_mutex>(*mEntries[componentId - 1].mutex));
    }

    template <typename T>
    void ComponentManager::add(uint32_t id, T value)
    {
        const std::size_t componentId = this->getID<T>();
        auto storage = static_cast<Storage<T>*>(mEntries[componentId - 1].storage.get());
        storage->insert(id, std::move(value));
    }

    template <typename T>
    void ComponentManager::remove(uint32_t id)
    {
        const std::size_t componentId = this->getID<T>();
        auto storage = static_cast<Storage<T>*>(mEntries[componentId - 1].storage.get());
        storage->erase(id);
    }
} // namespace cubos::core::ecs
