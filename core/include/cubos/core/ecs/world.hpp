/// @file
/// @brief Class @ref cubos::core::ecs::World.
/// @ingroup core-ecs

#pragma once

#include <cassert>
#include <cstddef>
#include <unordered_map>

#include <cubos/core/ecs/component/manager.hpp>
#include <cubos/core/ecs/entity/manager.hpp>
#include <cubos/core/ecs/resource/manager.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::ecs
{
    namespace impl
    {
        template <typename T>
        struct QueryFetcher;
    }

    /// @brief Holds entities, their components and resources.
    /// @see Internally, components are stored in abstract containers called @ref Storage's.
    /// @ingroup core-ecs
    class World
    {
    public:
        /// @brief Used to iterate over all entities in a world.
        using Iterator = EntityManager::Iterator;

        /// @brief Used to access the components in an entity.
        class Components;

        /// @brief Used to immutably access the components in an entity.
        class ConstComponents;

        /// @brief Constructs with the given @p initialCapacity.
        /// @param initialCapacity How many entities to reserve space for.
        World(std::size_t initialCapacity = 1024);

        /// @brief Registers and inserts a new resource type.
        /// @note Should be called before other operations, aside from @ref registerComponent().
        /// @tparam T Resource type.
        /// @tparam TArgs Types of the arguments of the constructor of the resource.
        /// @param args Arguments of the constructor of the resource.
        template <typename T, typename... TArgs>
        void registerResource(TArgs... args);

        /// @brief Registers a component type.
        /// @note Should be called before other operations, aside from @ref registerResource().
        /// @tparam T Component type.
        template <typename T>
        void registerComponent();

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

        /// @brief Creates a new entity with the given components.
        /// @tparam ComponentTypes Types of the components.
        /// @param components Initial values for the components.
        template <typename... ComponentTypes>
        Entity create(ComponentTypes... components);

        /// @brief Destroys an entity and its components.
        /// @todo Whats the behavior when we pass an entity that has already been destroyed?
        /// @param entity Entity identifier.
        void destroy(Entity entity);

        /// @brief Checks if an entity is still alive.
        /// @param entity Entity identifier.
        /// @return Whether the entity is alive.
        bool isAlive(Entity entity) const;

        /// @brief Adds components to an entity.
        ///
        /// If the entity already has one of the components, it is overwritten.
        ///
        /// @tparam ComponentTypes Types of the components.
        /// @param entity Entity identifier.
        /// @param components Initial values for the components.
        template <typename... ComponentTypes>
        void add(Entity entity, ComponentTypes&&... components);

        /// @brief Removes components from an entity.
        ///
        /// If the entity doesn't have one of the components, nothing happens.
        ///
        /// @tparam ComponentTypes Types of the components.
        /// @param entity Entity identifier.
        template <typename... ComponentTypes>
        void remove(Entity entity);

        /// @brief Creates a components view for the given entity.
        ///
        /// The given @p entity must be @ref isAlive "alive".
        ///
        /// @param entity Entity.
        /// @return Components view.
        Components components(Entity entity);

        /// @copydoc components(Entity)
        ConstComponents components(Entity entity) const;

        /// @brief Creates a package from the components of an entity.
        /// @param entity Entity identifier.
        /// @param context Optional context for serializing the components.
        /// @return Package containing the components of the entity.
        data::old::Package pack(Entity entity, data::old::Context* context = nullptr) const;

        /// @brief Unpacks components specified in a package into an entity.
        ///
        /// Removes any components that are already present in the entity.
        ///
        /// @param entity Entity identifier.
        /// @param package Package to unpack.
        /// @param context Optional context for deserializing the components.
        /// @return Whether the package was unpacked successfully.
        bool unpack(Entity entity, const data::old::Package& package, data::old::Context* context = nullptr);

        /// @brief Returns an iterator which points to the first entity of the world.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Returns an iterator which points to the end of the world.
        /// @return Iterator.
        Iterator end() const;

    private:
        template <typename... ComponentTypes>
        friend class Query;
        template <typename T>
        friend struct impl::QueryFetcher;
        friend class CommandBuffer;

        ResourceManager mResourceManager;
        EntityManager mEntityManager;
        ComponentManager mComponentManager;
    };

    class World::Components
    {
    public:
        /// @brief Constructs.
        /// @param world World.
        /// @param entity Entity.
        Components(World& world, Entity entity);

        /// @brief Checks if the given component is present.
        /// @param type Component type.
        /// @return Whether the component is present.
        bool has(const reflection::Type& type) const;

        /// @brief Checks if the given component is present.
        /// @tparam T Component type.
        /// @return Whether the component is present.
        template <reflection::Reflectable T>
        bool has() const
        {
            return this->has(reflection::reflect<T>());
        }

        /// @brief Returns a pointer to the component with the given type.
        ///
        /// The entity must @ref has(const reflection::Type&) "have" a component with the given
        /// @p type.
        ///
        /// @param type Component type.
        /// @return Pointer to component.
        void* get(const reflection::Type& type);

        /// @brief Returns a pointer to the component with the given type.
        ///
        /// The entity must @ref has() "have" a component of the type @p T.
        ///
        /// @tparam T Component type.
        /// @return Reference to component.
        template <reflection::Reflectable T>
        T& get()
        {
            return *static_cast<T*>(this->get(reflection::reflect<T>()));
        }

    private:
        World& mWorld;
        Entity mEntity;
    };

    class World::ConstComponents
    {
    public:
        /// @brief Constructs.
        /// @param world World.
        /// @param entity Entity.
        ConstComponents(const World& world, Entity entity);

        /// @brief Checks if the given component is present.
        /// @param type Component type.
        /// @return Whether the component is present.
        bool has(const reflection::Type& type) const;

        /// @brief Checks if the given component is present.
        /// @tparam T Component type.
        /// @return Whether the component is present.
        template <reflection::Reflectable T>
        bool has() const
        {
            return this->has(reflection::reflect<T>());
        }

        /// @brief Returns a pointer to the component with the given type.
        ///
        /// The entity must @ref has(const reflection::Type&) "have" a component with the given
        /// @p type.
        ///
        /// @param type Component type.
        /// @return Pointer to component.
        const void* get(const reflection::Type& type) const;

        /// @brief Returns a pointer to the component with the given type.
        ///
        /// The entity must @ref has() "have" a component of the type @p T.
        ///
        /// @tparam T Component type.
        /// @return Reference to component.
        template <reflection::Reflectable T>
        const T& get() const
        {
            return *static_cast<const T*>(this->get(reflection::reflect<T>()));
        }

    private:
        const World& mWorld;
        Entity mEntity;
    };

    // Implementation.

    template <typename T, typename... TArgs>
    void World::registerResource(TArgs... args)
    {
        CUBOS_TRACE("Registered resource '{}'", typeid(T).name());
        mResourceManager.add<T>(args...);
    }

    template <typename T>
    void World::registerComponent()
    {
        CUBOS_TRACE("Registered component '{}'", getComponentName<T>().value());
        mComponentManager.registerComponent<T>();
    }

    template <typename T>
    ReadResource<T> World::read() const
    {
        return mResourceManager.read<T>();
    }

    template <typename T>
    WriteResource<T> World::write() const
    {
        return mResourceManager.write<T>();
    }

    template <typename... ComponentTypes>
    Entity World::create(ComponentTypes... components)
    {
        std::size_t ids[] = {
            0, (mComponentManager
                    .getID<std::remove_const_t<std::remove_reference_t<std::remove_pointer_t<ComponentTypes>>>>())...};

        Entity::Mask mask{};
        for (auto id : ids)
        {
            mask.set(id);
        }

        auto entity = mEntityManager.create(mask);
        ([&](auto component) { mComponentManager.add(entity.index, std::move(component)); }(std::move(components)),
         ...);

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_DEBUG
        // Get the number of components being added.
        constexpr std::size_t ComponentCount = sizeof...(ComponentTypes);

        std::string componentNames[] = {"", "'" + std::string{getComponentName<ComponentTypes>().value()} + "'" ...};
        CUBOS_DEBUG("Created entity {} with components {}", entity.index,
                    fmt::join(componentNames + 1, componentNames + ComponentCount + 1, ", "));
#endif
        return entity;
    }

    template <typename... ComponentTypes>
    void World::add(Entity entity, ComponentTypes&&... components)
    {
        if (!mEntityManager.isValid(entity))
        {
            CUBOS_ERROR("Entity {} doesn't exist!", entity.index);
            return;
        }

        auto mask = mEntityManager.getMask(entity);

        (
            [&]() {
                std::size_t componentId = mComponentManager.getID<ComponentTypes>();
                mask.set(componentId);
                mComponentManager.add(entity.index, std::move(components));
            }(),
            ...);

        mEntityManager.setMask(entity, mask);

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_DEBUG
        std::string componentNames[] = {"'" + std::string{getComponentName<ComponentTypes>().value()} + "'" ...};
        CUBOS_DEBUG("Added components {} to entity {}", fmt::join(componentNames, ", "), entity.index);
#endif
    }

    template <typename... ComponentTypes>
    void World::remove(Entity entity)
    {
        if (!mEntityManager.isValid(entity))
        {
            CUBOS_ERROR("Entity {} doesn't exist!", entity.index);
            return;
        }

        auto mask = mEntityManager.getMask(entity);

        (
            [&]() {
                std::size_t componentId = mComponentManager.getID<ComponentTypes>();
                mask.set(componentId, false);
                mComponentManager.remove<ComponentTypes>(entity.index);
            }(),
            ...);

        mEntityManager.setMask(entity, mask);

#if CUBOS_LOG_LEVEL <= CUBOS_LOG_LEVEL_DEBUG
        std::string componentNames[] = {"'" + std::string{getComponentName<ComponentTypes>().value()} + "'" ...};
        CUBOS_DEBUG("Removed components {} from entity {}", fmt::join(componentNames, ", "), entity.index);
#endif
    }
} // namespace cubos::core::ecs
