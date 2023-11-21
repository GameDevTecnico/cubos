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
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/type_registry.hpp>

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
        /// @param type Component type.
        void registerComponent(const reflection::Type& type);

        /// @brief Registers a component type.
        /// @tparam T Component type.
        template <reflection::Reflectable T>
        void registerComponent()
        {
            this->registerComponent(reflection::reflect<T>());
        }

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

        /// @brief Creates a new entity.
        /// @return Entity.
        Entity create();

        /// @brief Destroys an entity.
        ///
        /// If an entity has already been destroyed, this function does nothing.
        ///
        /// @param entity Entity identifier.
        void destroy(Entity entity);

        /// @brief Checks if an entity is still alive.
        /// @param entity Entity.
        /// @return Whether the entity is alive.
        bool isAlive(Entity entity) const;

        /// @brief Returns a type registry which contains all registered component types.
        /// @return Component type registry.
        reflection::TypeRegistry components() const;

        /// @brief Creates a components view for the given entity.
        ///
        /// The given @p entity must be @ref isAlive "alive".
        ///
        /// @param entity Entity.
        /// @return Components view.
        Components components(Entity entity);

        /// @copydoc components(Entity)
        ConstComponents components(Entity entity) const;

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

    class World::Components final
    {
    public:
        /// @brief Used to iterate over the the components.
        class Iterator;

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

        /// @brief Returns an iterator to the first component.
        /// @return Iterator.
        Iterator begin();

        /// @brief Returns an iterator to the entry after the last component.
        /// @return Iterator.
        Iterator end();

        /// @brief Adds a component to the entity.
        ///
        /// If the entity already has the component, its value is overwritten.
        ///
        /// @param type Component type.
        /// @param value Component value to move.
        /// @return Reference to this.
        Components& add(const reflection::Type& type, void* value);

        /// @brief Adds a component to the entity.
        ///
        /// If the entity already has the component, its value is overwritten.
        ///
        /// @tparam T Component type.
        /// @param value Component value to move.
        /// @return Reference to this.
        template <reflection::Reflectable T>
        Components& add(T&& value)
        {
            return this->add(reflection::reflect<T>(), &value);
        }

        /// @brief Removes a component from the entity.
        ///
        /// If the entity doesn't have the component, nothing happens.
        ///
        /// @param type Component type.
        /// @return Reference to this.
        Components& remove(const reflection::Type& type);

        /// @brief Removes a component from the entity.
        ///
        /// If the entity doesn't have the component, nothing happens.
        ///
        /// @tparam T Component type.
        /// @return Reference to this.
        template <reflection::Reflectable T>
        Components& remove()
        {
            return this->remove(reflection::reflect<T>());
        }

    private:
        World& mWorld;
        Entity mEntity;
    };

    class World::ConstComponents final
    {
    public:
        /// @brief Used to iterate over the the components.
        class Iterator;

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

        /// @brief Returns an iterator to the first component.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Returns an iterator to the entry after the last component.
        /// @return Iterator.
        Iterator end() const;

    private:
        const World& mWorld;
        Entity mEntity;
    };

    class World::Components::Iterator final
    {
    public:
        /// @brief Output structure for the iterator.
        struct Component
        {
            const reflection::Type* type; ///< Component type.
            void* value;                  ///< Component value.
        };

        /// @brief Constructs.
        /// @param components Components view.
        /// @param end Whether the iterator represents the end or the beginning.
        Iterator(Components& components, bool end);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same component.
        bool operator==(const Iterator& other) const;

        /// @brief Accesses the component referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Component& operator*() const;

        /// @brief Accesses the component referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Component* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        World::Components& mComponents;
        uint32_t mId{1};
        mutable Component mComponent;
    };

    class World::ConstComponents::Iterator final
    {
    public:
        /// @brief Output structure for the iterator.
        struct Component
        {
            const reflection::Type* type; ///< Component type.
            const void* value;            ///< Component value.
        };

        /// @brief Constructs.
        /// @param components Components view.
        /// @param end Whether the iterator represents the end or the beginning.
        Iterator(const ConstComponents& components, bool end);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same component.
        bool operator==(const Iterator& other) const;

        /// @brief Accesses the component referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Component& operator*() const;

        /// @brief Accesses the component referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Component* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const World::ConstComponents& mComponents;
        uint32_t mId{1};
        mutable Component mComponent;
    };

    // Implementation.

    template <typename T, typename... TArgs>
    void World::registerResource(TArgs... args)
    {
        CUBOS_TRACE("Registered resource '{}'", typeid(T).name());
        mResourceManager.add<T>(args...);
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
} // namespace cubos::core::ecs
