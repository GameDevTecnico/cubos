/// @file
/// @brief Class @ref cubos::core::ecs::World.
/// @ingroup core-ecs

#pragma once

#include <cassert>
#include <cstddef>

#include <cubos/core/ecs/entity/archetype_graph.hpp>
#include <cubos/core/ecs/entity/pool.hpp>
#include <cubos/core/ecs/table/tables.hpp>
#include <cubos/core/ecs/types.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/string_view.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/type_registry.hpp>

namespace cubos::core::ecs
{
    class Observers;

    /// @brief Holds entities, their components and resources.
    /// @see Internally, components are stored in abstract containers called @ref Storage's.
    /// @ingroup core-ecs
    class World
    {
    public:
        ~World();

        /// @brief World constructor.
        World();

        /// @brief Used to access the components in an entity.
        class Components;

        /// @brief Used to immutably access the components in an entity.
        class ConstComponents;

        /// @brief Used to access the relations of an entity.
        class Relations;

        /// @brief Used to immutably access the relations of an entity.
        class ConstRelations;

        /// @brief Registers a resource type.
        /// @param type Resource type.
        void registerResource(const reflection::Type& type);

        /// @brief Registers a resource type.
        /// @tparam T Resource type.
        template <typename T>
        void registerResource()
        {
            this->registerResource(reflection::reflect<T>());
        }

        /// @brief Registers a component type.
        /// @param type Component type.
        void registerComponent(const reflection::Type& type);

        /// @brief Registers a component type.
        /// @tparam T Component type.
        template <typename T>
        void registerComponent()
        {
            this->registerComponent(reflection::reflect<T>());
        }

        /// @brief Registers a relation type.
        /// @param type Relation type.
        void registerRelation(const reflection::Type& type);

        /// @brief Registers a relation type.
        /// @tparam T Relation type.
        template <typename T>
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

        /// @brief Returns the observers of the world.
        /// @return Observers.
        Observers& observers();

        /// @copydoc observers()
        const Observers& observers() const;

        /// @brief Inserts a new resource into the world.
        /// @param value Resource value.
        void insertResource(memory::AnyValue value);

        /// @brief Inserts a new resource into the world.
        /// @tparam T Resource type.
        /// @param value Resource value.
        template <typename T>
        void insertResource(T value)
        {
            this->insertResource(memory::AnyValue::moveConstruct(reflection::reflect<T>(), &value));
        }

        /// @brief Erases a new resource from the world.
        /// @param type Resource type.
        /// @return Whether there was such a resource.
        bool eraseResource(const reflection::Type& type);

        /// @brief Erases a new resource from the world.
        /// @tparam T Resource type.
        /// @return Whether there was such a resource.
        template <typename T>
        bool eraseResource()
        {
            return this->eraseResource(reflection::reflect<T>());
        }

        /// @brief Checks if a resource is present.
        /// @param type Resource type.
        /// @return Whether the resource is present.
        bool hasResource(const reflection::Type& type) const;

        /// @brief Checks if a resource is present.
        /// @tparam T Resource type.
        /// @return Whether the resource is present.
        template <typename T>
        bool hasResource() const
        {
            return this->hasResource(reflection::reflect<T>());
        }

        /// @brief Returns a pointer to a resource.
        /// @param type Resource type.
        /// @return Resource.
        void* resource(const reflection::Type& type);

        /// @brief Returns a reference to a resource.
        /// @tparam T Resource type.
        /// @return Resource.
        template <typename T>
        T& resource()
        {
            return *static_cast<T*>(this->resource(reflection::reflect<T>()));
        }

        /// @brief Returns a pointer to a resource.
        /// @param type Resource type.
        /// @return Resource.
        const void* resource(const reflection::Type& type) const;

        /// @brief Returns a reference to a resource.
        /// @tparam T Resource type.
        /// @return Resource.
        template <typename T>
        const T& resource() const
        {
            return *static_cast<const T*>(this->resource(reflection::reflect<T>()));
        }

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

        /// @brief Creates a view for relations coming from the given entity.
        ///
        /// The given @p entity must be @ref isAlive "alive".
        ///
        /// @param entity From entity.
        /// @return Relations view.
        Relations relationsFrom(Entity entity);

        /// @copydoc relationsFrom(Entity)
        ConstRelations relationsFrom(Entity entity) const;

        /// @brief Creates a view for relations going into the given entity.
        ///
        /// The given @p entity must be @ref isAlive "alive".
        ///
        /// @param entity To entity.
        /// @return Relations view.
        Relations relationsTo(Entity entity);

        /// @copydoc relationsTo(Entity)
        ConstRelations relationsTo(Entity entity) const;

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
        template <typename T>
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
        template <typename T>
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

        /// @brief Gets the number of alive entities.
        /// @return Number of alive entities.
        std::size_t entityCount() const;

    private:
        /// @brief Called when an entity's depth changes. Propagates the change to incoming relations.
        /// @param index To entity index.
        /// @param dataType Tree relation data type.
        /// @param depth New depth.
        void propagateDepth(uint32_t index, DataTypeId dataType, int depth);

        /// @brief Moves tables in sparse data for the given entity to the given archetype.
        /// @param entity Entity.
        /// @param oldArchetype Old archetype.
        /// @param newArchetype New archetype.
        void moveSparse(Entity entity, ArchetypeId oldArchetype, ArchetypeId newArchetype);

        /// @brief Used to prevent cycles from forming in tree relations.
        /// @param index Entity index.
        /// @param ancestorIndex Ancestor entity index.
        /// @param dataType Tree relation data type.
        /// @return Whether @p ancestorIndex is an ancestor of @p index with the given tree relation.
        bool isAncestor(uint32_t index, uint32_t ancestorIndex, DataTypeId dataType) const;

        Types mTypes;
        EntityPool mEntityPool;
        ArchetypeGraph mArchetypeGraph;
        Tables mTables;
        Observers* mObservers;
        memory::TypeMap<memory::AnyValue> mResources;
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
        template <typename T>
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
        template <typename T>
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
        template <typename T>
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
        template <typename T>
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
        template <typename T>
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
        template <typename T>
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

    class World::Relations final
    {
    public:
        /// @brief Used to iterate over the the relations.
        class Iterator;

        /// @brief Constructs.
        /// @param world World.
        /// @param entity Entity.
        /// @param from Whether the relations are from or to the entity.
        Relations(World& world, Entity entity, bool from);

        /// @brief Checks if the entity is related to another entity with the given type.
        /// @param type Relation type.
        /// @param entity Entity.
        /// @return Whether the relation is present.
        bool has(const reflection::Type& type, Entity entity) const;

        /// @brief Checks if the entity is related to another entity with the given type.
        /// @tparam T Relation type.
        /// @param entity Entity.
        /// @return Whether the relation is present.
        template <typename T>
        bool has(Entity entity) const
        {
            return this->has(reflection::reflect<T>(), entity);
        }

        /// @brief Returns a pointer to the given relation.
        ///
        /// The entity must be @ref has(const reflection::Type&, Entity) "related" with the given @p entity and with the
        /// given @p type.
        ///
        /// @param type Relation type.
        /// @param entity Entity.
        /// @return Pointer to relation.
        void* get(const reflection::Type& type, Entity entity);

        /// @brief Returns a pointer to the given relation.
        ///
        /// The entity must be @ref has(Entity) "related" with the given @p entity and with the
        /// given @p type.
        ///
        /// @tparam T Relation type.
        /// @param entity Entity.
        /// @return Relation.
        template <typename T>
        T& get(Entity entity)
        {
            return *static_cast<T*>(this->get(reflection::reflect<T>(), entity));
        }

        /// @brief Returns an iterator to the first relation.
        /// @return Iterator.
        Iterator begin();

        /// @brief Returns an iterator to the entry after the last relation.
        /// @return Iterator.
        Iterator end();

    private:
        World& mWorld;
        Entity mEntity;
        bool mFrom;
    };

    class World::ConstRelations final
    {
    public:
        /// @brief Used to iterate over the the relations.
        class Iterator;

        /// @brief Constructs.
        /// @param world World.
        /// @param entity Entity.
        /// @param from Whether the relations are from or to the entity.
        ConstRelations(const World& world, Entity entity, bool from);

        /// @brief Checks if the entity is related to another entity with the given type.
        /// @param type Relation type.
        /// @param entity Entity.
        /// @return Whether the relation is present.
        bool has(const reflection::Type& type, Entity entity) const;

        /// @brief Checks if the entity is related to another entity with the given type.
        /// @tparam T Relation type.
        /// @param entity Entity.
        /// @return Whether the relation is present.
        template <typename T>
        bool has(Entity entity) const
        {
            return this->has(reflection::reflect<T>(), entity);
        }

        /// @brief Returns a pointer to the given relation.
        ///
        /// The entity must be @ref has(const reflection::Type&, Entity) "related" with the given @p entity and with the
        /// given @p type.
        ///
        /// @param type Relation type.
        /// @param entity Entity.
        /// @return Pointer to relation.
        const void* get(const reflection::Type& type, Entity entity);

        /// @brief Returns a pointer to the given relation.
        ///
        /// The entity must be @ref has(Entity) "related" with the given @p entity and with the
        /// given @p type.
        ///
        /// @tparam T Relation type.
        /// @param entity Entity.
        /// @return Relation.
        template <typename T>
        const T& get(Entity entity)
        {
            return *static_cast<const T*>(this->get(reflection::reflect<T>(), entity));
        }

        /// @brief Returns an iterator to the first relation.
        /// @return Iterator.
        Iterator begin();

        /// @brief Returns an iterator to the entry after the last relation.
        /// @return Iterator.
        Iterator end();

    private:
        const World& mWorld;
        Entity mEntity;
        bool mFrom;
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

    class World::Relations::Iterator final
    {
    public:
        /// @brief Output structure for the iterator.
        struct Relation
        {
            const reflection::Type* type; ///< Relation type.
            void* value;                  ///< Relation value.
            Entity entity;                ///< Related entity.
        };

        /// @brief Constructs.
        /// @param relations Relations view.
        /// @param end Whether the iterator represents the end or the beginning.
        Iterator(Relations& relations, bool end);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same relation.
        bool operator==(const Iterator& other) const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Relation& operator*() const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Relation* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        /// @brief Advances the iterator to the next match.
        void advance();

        World::Relations& mRelations;
        SparseRelationTableRegistry::Iterator mIterator;
        bool mReverse{false};
        std::size_t mTableIndex{SIZE_MAX};
        SparseRelationTableId mTableId;
        std::size_t mRow{SIZE_MAX};
        mutable Relation mRelation;
    };

    class World::ConstRelations::Iterator final
    {
    public:
        /// @brief Output structure for the iterator.
        struct Relation
        {
            const reflection::Type* type; ///< Relation type.
            const void* value;            ///< Relation value.
            Entity entity;                ///< Related entity.
        };

        /// @brief Constructs.
        /// @param relations Relations view.
        /// @param end Whether the iterator represents the end or the beginning.
        Iterator(ConstRelations& relations, bool end);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same relation.
        bool operator==(const Iterator& other) const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Relation& operator*() const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Entry.
        const Relation* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        /// @brief Advances the iterator to the next match.
        void advance();

        World::ConstRelations& mRelations;
        SparseRelationTableRegistry::Iterator mIterator;
        bool mReverse{false};
        std::size_t mTableIndex{SIZE_MAX};
        SparseRelationTableId mTableId;
        std::size_t mRow{SIZE_MAX};
        mutable Relation mRelation;
    };
} // namespace cubos::core::ecs
