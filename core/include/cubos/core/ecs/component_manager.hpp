#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <typeindex>

#include <cubos/core/ecs/storage.hpp>

namespace cubos::core::ecs
{
    /// Gets the registered name of a component type.
    /// If the component type is not registered, aborts the program.
    /// @tparam T Component type.
    /// @returns The registered name of the component type.
    template <typename T>
    std::optional<std::string_view> getComponentName();

    /// Gets the registered name of a component type.
    /// If the component type is not registered, aborts the program.
    /// @param type Component type.
    /// @returns The registered name of the component type.
    std::optional<std::string_view> getComponentName(std::type_index type);

    /// Utility struct used to reference component storages for reading.
    template <typename T>
    class ReadStorage
    {
    public:
        ReadStorage(ReadStorage&& /*other*/) noexcept;

        /// Gets the underlying storage reference.
        const Storage<T>& get() const;

    private:
        friend class ComponentManager;

        ReadStorage(const Storage<T>& storage, std::shared_lock<std::shared_mutex>&& lock);

        const Storage<T>& mStorage;
        std::shared_lock<std::shared_mutex> mLock;
    };

    /// Utility struct used to reference component storages for writing.
    template <typename T>
    class WriteStorage
    {
    public:
        WriteStorage(WriteStorage&& /*other*/) noexcept;

        /// Gets the underlying storage reference.
        Storage<T>& get() const;

    private:
        friend class ComponentManager;

        WriteStorage(Storage<T>& storage, std::unique_lock<std::shared_mutex>&& lock);

        Storage<T>& mStorage;
        std::unique_lock<std::shared_mutex> mLock;
    };

    class ComponentManager final
    {
    public:
        /// Registers a new component type with the component manager.
        /// Must be called before any component of this type is used in any way.
        /// @tparam T The type of the component.
        template <typename T>
        void registerComponent();

        /// Registers a new component type with the component manager.
        /// Must be called before any component of this type is used in any way.
        /// @param type The type of the component.
        void registerComponent(std::type_index type);

        /// Gets the ID of the component type.
        /// @param type The type of the component.
        /// @returns The ID of the component type.
        std::size_t getIDFromIndex(std::type_index type) const;

        /// Gets the ID of the component type.
        /// @tparam T The type of the component.
        /// @returns The ID of the component type.
        template <typename T>
        std::size_t getID() const;

        /// Gets the type of the component.
        /// @param id The ID of the component type.
        /// @returns The type of the component.
        std::type_index getType(std::size_t id) const;

        /// Gets the storage of the component type.
        /// @tparam T The type of the component.
        /// @returns The storage guard of the component type.
        template <typename T>
        ReadStorage<T> read() const;

        /// Gets the storage of the component type.
        /// @tparam T The type of the component.
        /// @returns The storage guard of the component type.
        template <typename T>
        WriteStorage<T> write() const;

        /// Adds a component to an entity.
        /// @tparam T The type of the component.
        /// @param id The ID of the entity.
        /// @param value The initial value of the component.
        template <typename T>
        void add(uint32_t id, T value);

        /// Removes a component from an entity.
        /// @tparam T The type of the component.
        /// @param id The ID of the entity.
        template <typename T>
        void remove(uint32_t id);

        /// Removes a component from an entity.
        /// @param id The ID of the entity.
        /// @param componentId The ID of the component.
        void remove(uint32_t id, std::size_t componentId);

        /// Removes all components from an entity.
        /// @param id The ID of the entity.
        void removeAll(uint32_t id);

        /// @brief Creates a package from a component of an entity.
        /// @param id Entity ID.
        /// @param componentId Component ID.
        /// @param context Context to use for serialization.
        /// @returns A package containing the component entity.
        data::Package pack(uint32_t id, std::size_t componentId, data::Context& context) const;

        /// @brief Inserts a component into an entity, by unpacking a package.
        /// @param id Entity ID.
        /// @param componentId Component ID.
        /// @param package Package to unpack.
        /// @param context Context to use for deserialization.
        /// @returns True if the package was unpacked successfully, false otherwise.
        bool unpack(uint32_t id, std::size_t componentId, const data::Package& package, data::Context& context);

    private:
        struct Entry
        {
            Entry(std::unique_ptr<IStorage> storage);

            std::unique_ptr<IStorage> storage;        ///< The generic component storage.
            std::unique_ptr<std::shared_mutex> mutex; ///< Read/write lock for the storage.
        };

        /// Maps component types to component IDs.
        std::unordered_map<std::type_index, std::size_t> mTypeToIds;

        std::vector<Entry> mEntries; ///< The registered component storages.
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
