/// @file
/// @brief Class @ref cubos::core::ecs::World.
/// @ingroup core-ecs

#pragma once

#include <cassert>
#include <cstddef>

#include <cubos/core/ecs/entity/archetype_graph.hpp>
#include <cubos/core/ecs/entity/pool.hpp>
#include <cubos/core/ecs/resource/manager.hpp>
#include <cubos/core/ecs/table/tables.hpp>
#include <cubos/core/ecs/types.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/type_registry.hpp>

namespace cubos::core::ecs
{
    /// @brief Holds entities, their components and resources.
    /// @see Internally, components are stored in abstract containers called @ref Storage's.
    /// @ingroup core-ecs
    class World
    {
    public:
        /// @brief World constructor.
        World();

        /// @brief Used to access the components in an entity.
        class Components;

        /// @brief Used to immutably access the components in an entity.
        class ConstComponents;

        /// @brief Registers and inserts a new resource type.
        /// @note Should be called before other non-registering operations.
        /// @tparam T Resource type.
        /// @tparam TArgs Types of the arguments of the constructor of the resource.
        /// @param args Arguments of the constructor of the resource.
        template <typename T, typename... TArgs>
        void registerResource(TArgs... args);

        /// @brief Registers a component type.
        /// @param type Component type.
        void registerComponent(const reflection::Type& type);

        /// @brief Registers a component type.
        /// @note Should be called before other non-registering operations.
        /// @tparam T Component type.
        template <reflection::Reflectable T>
        void registerComponent()
        {
            this->registerComponent(reflection::reflect<T>());
        }

        /// @brief Registers a relation type.
        /// @param type Relation type.
        void registerRelation(const reflection::Type& type);

        /// @brief Registers a relation type.
        /// @tparam T Relation type.
        template <reflection::Reflectable T>
        void registerRelation()
        {
            this->registerRelation(reflection::reflect<T>());
        }

        /// @brief Returns the types registry of the world.
        /// @return Types registry.
        Types& types();

        /// @copydoc types()
        const Types& types() const;

        /// @brief Returns the tables of the world.
        /// @return Tables.
        Tables& tables();

        /// @copydoc tables()
        const Tables& tables() const;

        /// @brief Returns the archetype graph of the world.
        /// @return Archetype graph.
        ArchetypeGraph& archetypeGraph();

        /// @copydoc archetypeGraph()
        const ArchetypeGraph& archetypeGraph() const;

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
        /// @return Entity identifier.
        Entity create();

        /// @brief Creates a new entity with the given identifier.
        ///
        /// Aborts if the entity already exists.
        ///
        /// @param entity Entity identifier.
        /// @return Entity.
        void createAt(Entity entity);

        /// @brief Reserves an entity identifier, without actually creating an entity.
        ///
        /// The entity can be later created with a call to @ref createAt().
        ///
        /// @return Entity identifier.
        Entity reserve();

        /// @brief Destroys an entity.
        ///
        /// If an entity has already been destroyed, this function does nothing.
        ///
        /// @param entity Entity identifier.
        void destroy(Entity entity);

        /// @brief Gets the generation of the entity with the given index.
        /// @param index Entity index.
        /// @return Entity generation.
        uint32_t generation(uint32_t index) const;

        /// @brief Gets the archetype of the given entity.
        /// @param entity Entity.
        /// @return Archetype identifier.
        ArchetypeId archetype(Entity entity) const;

        /// @brief Checks if an entity is still alive.
        /// @param entity Entity.
        /// @return Whether the entity is alive.
        bool isAlive(Entity entity) const;

        /// @brief Creates a components view for the given entity.
        ///
        /// The given @p entity must be @ref isAlive "alive".
        ///
        /// @param entity Entity.
        /// @return Components view.
        Components components(Entity entity);

        /// @copydoc components(Entity)
        ConstComponents components(Entity entity) const;

        /// @brief Inserts a relation between the two given entities.
        ///
        /// If the relation already exists, it is overwritten.
        ///
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        /// @param value Relation value to move.
        void relate(Entity from, Entity to, const reflection::Type& type, void* value);

        /// @brief Inserts a relation between the two given entities.
        ///
        /// If the relation already exists, it is overwritten.
        ///
        /// @tparam T Relation type.
        /// @param from From entity.
        /// @param to To entity.
        /// @param value Relation value.
        template <reflection::Reflectable T>
        void relate(Entity from, Entity to, T value)
        {
            this->relate(from, to, reflection::reflect<T>(), &value);
        }

        /// @brief Removes the relation, if there's any, between the two given entities.
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        void unrelate(Entity from, Entity to, const reflection::Type& type);

        /// @brief Removes the relation, if there's any, between the two given entities.
        /// @tparam T Relation type.
        /// @param from From entity.
        /// @param to To entity.
        template <reflection::Reflectable T>
        void unrelate(Entity from, Entity to)
        {
            this->unrelate(from, to, reflection::reflect<T>());
        }

        /// @brief Checks if there's a relation of the given type between the two given entities.
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        /// @return Whether the relation exists.
        bool related(Entity from, Entity to, const reflection::Type& type) const;

        /// @brief Checks if there's a relation of the given type between the two given entities.
        /// @tparam T Relation type.
        /// @param from From entity.
        /// @param to To entity.
        /// @return Whether the relation exists.
        template <typename T>
        bool related(Entity from, Entity to) const
        {
            return this->related(from, to, reflection::reflect<T>());
        }

        /// @brief Gets a pointer to the relation value between the two given entities.
        ///
        /// The relation must @ref related() "exist".
        ///
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        /// @return Pointer to relation value.
        void* relation(Entity from, Entity to, const reflection::Type& type);

        /// @copydoc relation(Entity, Entity, const reflection::Type&)
        const void* relation(Entity from, Entity to, const reflection::Type& type) const;

        /// @brief Gets a reference to the relation value between the two given entities.
        ///
        /// The relation must @ref related() "exist".
        ///
        /// @tparam T Relation type.
        /// @param from From entity.
        /// @param to To entity.
        /// @return Reference to relation value.
        template <typename T>
        T& relation(Entity from, Entity to)
        {
            return *static_cast<T*>(this->relation(from, to, reflection::reflect<T>()));
        }

        /// @copydoc relation(Entity, Entity)
        template <typename T>
        const T& relation(Entity from, Entity to) const
        {
            return *static_cast<const T*>(this->relation(from, to, reflection::reflect<T>()));
        }

    private:
        /// @brief Called when an entity's depth changes. Propagates the change to incoming relations.
        /// @param index To entity index.
        /// @param dataType Tree relation data type.
        /// @param depth New depth.
        void propagateDepth(uint32_t index, DataTypeId dataType, int depth);

        Types mTypes;
        EntityPool mEntityPool;
        ArchetypeGraph mArchetypeGraph;
        Tables mTables;

        ResourceManager mResourceManager;
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
        DataTypeId mId;
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
        DataTypeId mId;
        mutable Component mComponent;
    };

    // Implementation.

    template <typename T, typename... TArgs>
    void World::registerResource(TArgs... args)
    {
        CUBOS_TRACE("Registered resource {}", typeid(T).name());
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
