#ifndef CUBOS_CORE_ECS_COMPONENT_MANAGER_HPP
#define CUBOS_CORE_ECS_COMPONENT_MANAGER_HPP

#include <cubos/core/ecs/storage.hpp>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <shared_mutex>

namespace cubos::core::ecs
{
    /// Utility struct used to reference component storages for reading.
    template <typename T> class ReadStorage
    {
    public:
        ReadStorage(ReadStorage&&);

        /// Gets the underlying storage reference.
        const typename T::Storage& get() const;

    private:
        friend class ComponentManager;

        ReadStorage(const typename T::Storage& storage, std::shared_lock<std::shared_mutex>&& lock);

        const typename T::Storage& storage;
        std::shared_lock<std::shared_mutex> lock;
    };

    /// Utility struct used to reference component storages for writing.
    template <typename T> class WriteStorage
    {
    public:
        WriteStorage(WriteStorage&&);

        /// Gets the underlying storage reference.
        typename T::Storage& get() const;

    private:
        friend class ComponentManager;

        WriteStorage(typename T::Storage& resource, std::unique_lock<std::shared_mutex>&& lock);

        typename T::Storage& storage;
        std::unique_lock<std::shared_mutex> lock;
    };

    /// Object which stores temporary component data for use with the Commands object.
    /// Used internally by the ComponentManager.
    /// @tparam T The type of the component.
    template <typename T> class StagingArea
    {
    public:
    private:
    };

    class ComponentManager final
    {
    public:
        ComponentManager();
        ~ComponentManager();

        /// Registers a new component type with the component manager.
        /// Must be called before any component of this type is used in any way.
        /// @tparam T The type of the component.
        template <typename T> void registerComponent();

        /// Gets the ID of the component type.
        /// @tparam T The type of the component.
        /// @returns The ID of the component type.
        template <typename T> size_t getID() const;

        /// Gets the storage of the component type.
        /// @tparam T The type of the component.
        /// @returns The storage guard of the component type.
        template <typename T> ReadStorage<T> read() const;

        /// Gets the storage of the component type.
        /// @tparam T The type of the component.
        /// @returns The storage guard of the component type.
        template <typename T> WriteStorage<T> write() const;

        /// Stages a component to be added to an entity.
        /// @tparam T The type of the component.
        /// @param id The ID of the entity.
        /// @param value The initial value of the component.
        template <typename T> void add(uint32_t id, T&& value) const;

        /// Stages a component to be removed from an entity.
        /// @tparam T The type of the component.
        /// @param id The ID of the entity.
        template <typename T> void remove(uint32_t id) const;

        /// Commits the staged components to their storages.
        void commit();

    private:
        struct Entry
        {
            IStorage* storage;       ///< The generic component storage.
            void* staging;           ///< The staging area for the component.
            std::shared_mutex mutex; ///< Read/write lock for the storage.

            Entry(IStorage* storage, void* staging);
        };

        /// Utility function which returns the global ID of a component.
        /// @tparam T Component type.
        /// @return Global ID of the component.
        template <typename T> static size_t getGlobalComponentId();

        /// Keeps track of the next available global component ID.
        static size_t nextGlobalComponentId;

        /// Maps global component IDs to local component IDs.
        std::unordered_map<size_t, size_t> globalToLocalComponentIds;

        std::vector<Entry> entries; ///< The registered component storages.
    };

    // Implementation.

    template <typename T>
    ReadStorage<T>::ReadStorage(ReadStorage&& other) : storage(other.storage), lock(std::move(other.lock))
    {
        // Do nothing.
    }

    template <typename T> const typename T::Storage& ReadStorage<T>::get() const
    {
        return this->storage;
    }

    template <typename T>
    ReadStorage<T>::ReadStorage(const typename T::Storage& storage, std::shared_lock<std::shared_mutex>&& lock)
        : storage(storage), lock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T>
    WriteStorage<T>::WriteStorage(WriteStorage&& other) : storage(other.storage), lock(std::move(other.lock))
    {
        // Do nothing.
    }

    template <typename T> typename T::Storage& WriteStorage<T>::get() const
    {
        return this->storage;
    }

    template <typename T>
    WriteStorage<T>::WriteStorage(typename T::Storage& storage, std::unique_lock<std::shared_mutex>&& lock)
        : storage(storage), lock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T> void ComponentManager::registerComponent()
    {
        static_assert(std::is_same<T, typename T::Storage::Type>(),
                      "A component can't use a storage for a different component type!");
        const size_t globalComponentId = ComponentManager::getGlobalComponentId<T>();
        if (this->globalToLocalComponentIds.find(globalId) == this->globalToLocalComponentIds.end())
        {
            this->globalToLocalComponentIds[globalComponentId] = this->storages.size();
            this->entries.emplace_back(new typename T::Storage());
        }
    }

    template <typename T> size_t ComponentManager::getID() const
    {
        const size_t globalComponentId = ComponentManager::getGlobalComponentId<T>();
        return this->globalToLocalComponentIds.at(globalComponentId);
    }

    template <typename T> ReadStorage<T> ComponentManager::read() const
    {
        const size_t id = this->getID<T>();
        return ReadStorage<T>(*static_cast<const typename T::Storage*>(this->entries[id].storage),
                              std::shared_lock<std::shared_mutex>(this->entries[id].mutex));
    }

    template <typename T> WriteStorage<T> ComponentManager::write() const
    {
        const size_t id = this->getID<T>();
        return WriteStorage<T>(*static_cast<typename T::Storage*>(this->entries[id].storage),
                               std::unique_lock<std::shared_mutex>(this->entries[id].mutex));
    }

    template <typename T> void ComponentManager::add(uint32_t id, T&& value) const
    {
        const size_t id = this->getID<T>();
        this->entries[id].storage->add(id, std::forward<T>(value));
    }

    template <typename T> void ComponentManager::remove(uint32_t id) const
    {
        const size_t id = this->getID<T>();
        this->entries[id].storage->remove(id);
    }

    void ComponentManager::commit()
    {
        for (auto& entry : this->entries)
        {
            entry.storage->commit();
        }
    }

    ComponentManager::Entry::Entry(IStorage* storage, void* staging) : storage(storage), staging(staging)
    {
        // Do nothing.
    }

} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_COMPONENT_MANAGER_HPP
