#ifndef CUBOS_CORE_ECS_COMPONENT_MANAGER_HPP
#define CUBOS_CORE_ECS_COMPONENT_MANAGER_HPP

#include <cubos/core/ecs/storage.hpp>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <shared_mutex>
#include <typeindex>
#include <memory>
#include <mutex>

namespace cubos::core::ecs
{
    /// Generic type used to identify the storage type of a component.
    /// Must be specialized for each component type.
    template <typename T>
    struct ComponentStorage
    {
        // This should never be instantiated.
        static_assert(!std::is_same_v<T, T>, "ComponentStorage must be specialized for each component type.");
    };

    template <typename T>
    concept ComponentUsingStorage = requires
    {
        typename T::Storage;
    };

    template <typename T>
    requires ComponentUsingStorage<T>
    struct ComponentStorage<T>
    {
        using Type = typename T::Storage;
    };

    /// Gets the registered name of a component type.
    /// If the component type is not registered, aborts the program.
    /// @tparam T Component type.
    /// @returns The registered name of the component type.
    template <typename T>
    const std::string& getComponentName();

    /// Gets the registered name of a component type.
    /// If the component type is not registered, aborts the program.
    /// @param type Component type.
    /// @returns The registered name of the component type.
    const std::string& getComponentName(std::type_index type);

    /// Utility struct used to reference component storages for reading.
    template <typename T>
    class ReadStorage
    {
    public:
        ReadStorage(ReadStorage&&);

        /// Gets the underlying storage reference.
        const typename ComponentStorage<T>::Type& get() const;

    private:
        friend class ComponentManager;

        ReadStorage(const typename ComponentStorage<T>::Type& storage, std::shared_lock<std::shared_mutex>&& lock);

        const typename ComponentStorage<T>::Type& storage;
        std::shared_lock<std::shared_mutex> lock;
    };

    /// Utility struct used to reference component storages for writing.
    template <typename T>
    class WriteStorage
    {
    public:
        WriteStorage(WriteStorage&&);

        /// Gets the underlying storage reference.
        typename ComponentStorage<T>::Type& get() const;

    private:
        friend class ComponentManager;

        WriteStorage(typename ComponentStorage<T>::Type& resource, std::unique_lock<std::shared_mutex>&& lock);

        typename ComponentStorage<T>::Type& storage;
        std::unique_lock<std::shared_mutex> lock;
    };

    class ComponentManager final
    {
    public:
        ~ComponentManager();

        /// Registers a new component type with the component manager.
        /// Must be called before any component of this type is used in any way.
        /// @tparam T The type of the component.
        template <typename T>
        void registerComponent();

        /// Gets the ID of the component type.
        /// @param type The type of the component.
        /// @returns The ID of the component type.
        size_t getIDFromIndex(std::type_index type) const;

        /// Gets the ID of the component type.
        /// @tparam T The type of the component.
        /// @returns The ID of the component type.
        template <typename T>
        size_t getID() const;

        /// Gets the type of the component.
        /// @param id The ID of the component type.
        /// @returns The type of the component.
        std::type_index getType(size_t id) const;

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
        void remove(uint32_t id, size_t componentId);

        /// Removes all components from an entity.
        /// @param id The ID of the entity.
        void removeAll(uint32_t id);

        /// @brief Creates a package from a component of an entity.
        /// @param id Entity ID.
        /// @param componentId Component ID.
        /// @param map Map of component IDs to package IDs.
        /// @param handleCtx Handle serialization context.
        /// @returns A package containing the component entity.
        data::Package pack(uint32_t id, size_t componentId, const data::SerializationMap<Entity, std::string>& map,
                           data::Handle::SerContext handleCtx) const;

        /// @brief Inserts a component into an entity, by unpacking a package.
        /// @param id Entity ID.
        /// @param componentId Component ID.
        /// @param package Package to unpack.
        /// @param map Map of component IDs to package IDs.
        /// @param handleCtx Handle deserialization context.
        /// @returns True if the package was unpacked successfully, false otherwise.
        bool unpack(uint32_t id, size_t componentId, const data::Package& package,
                    const data::SerializationMap<Entity, std::string>& map, data::Handle::DesContext handleCtx);

    private:
        struct Entry
        {
            IStorage* storage;                        ///< The generic component storage.
            std::unique_ptr<std::shared_mutex> mutex; ///< Read/write lock for the storage.

            Entry(IStorage* storage);
        };

        /// Maps component types to component IDs.
        std::unordered_map<std::type_index, size_t> typeToIds;

        std::vector<Entry> entries; ///< The registered component storages.
    };

    // Implementation.

    template <typename T>
    const std::string& getComponentName()
    {
        return getComponentName(typeid(T));
    }

    template <typename T>
    ReadStorage<T>::ReadStorage(ReadStorage&& other) : storage(other.storage), lock(std::move(other.lock))
    {
        // Do nothing.
    }

    template <typename T>
    const typename ComponentStorage<T>::Type& ReadStorage<T>::get() const
    {
        return this->storage;
    }

    template <typename T>
    ReadStorage<T>::ReadStorage(const typename ComponentStorage<T>::Type& storage,
                                std::shared_lock<std::shared_mutex>&& lock)
        : storage(storage), lock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T>
    WriteStorage<T>::WriteStorage(WriteStorage&& other) : storage(other.storage), lock(std::move(other.lock))
    {
        // Do nothing.
    }

    template <typename T>
    typename ComponentStorage<T>::Type& WriteStorage<T>::get() const
    {
        return this->storage;
    }

    template <typename T>
    WriteStorage<T>::WriteStorage(typename ComponentStorage<T>::Type& storage,
                                  std::unique_lock<std::shared_mutex>&& lock)
        : storage(storage), lock(std::move(lock))
    {
        // Do nothing.
    }

    template <typename T>
    void ComponentManager::registerComponent()
    {
        static_assert(std::is_same<T, typename ComponentStorage<T>::Type::Type>(),
                      "A component can't use a storage for a different component type!");
        const auto type = std::type_index(typeid(T));
        if (this->typeToIds.find(type) == this->typeToIds.end())
        {
            this->typeToIds[type] = this->entries.size() + 1; // Component ids start at 1.
            this->entries.emplace_back(new typename ComponentStorage<T>::Type());
        }
    }

    template <typename T>
    size_t ComponentManager::getID() const
    {
        return this->getIDFromIndex(typeid(T));
    }

    template <typename T>
    ReadStorage<T> ComponentManager::read() const
    {
        const size_t id = this->getID<T>();
        return ReadStorage<T>(*static_cast<const typename ComponentStorage<T>::Type*>(this->entries[id - 1].storage),
                              std::shared_lock<std::shared_mutex>(*this->entries[id - 1].mutex));
    }

    template <typename T>
    WriteStorage<T> ComponentManager::write() const
    {
        const size_t id = this->getID<T>();
        return WriteStorage<T>(*static_cast<typename ComponentStorage<T>::Type*>(this->entries[id - 1].storage),
                               std::unique_lock<std::shared_mutex>(*this->entries[id - 1].mutex));
    }

    template <typename T>
    void ComponentManager::add(uint32_t entityId, T value)
    {
        const size_t id = this->getID<T>();
        auto storage = static_cast<typename ComponentStorage<T>::Type*>(this->entries[id - 1].storage);
        storage->insert(entityId, std::move(value));
    }

    template <typename T>
    void ComponentManager::remove(uint32_t entityId)
    {
        const size_t id = this->getID<T>();
        auto storage = static_cast<typename ComponentStorage<T>::Type*>(this->entries[id - 1].storage);
        storage->erase(entityId);
    }
} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_COMPONENT_MANAGER_HPP
