# Queries {#features-queries}

@brief In-depth guide on writing queries

@m_footernavigation

@note This guide assumes you've read the @ref features-ecs "ECS" guide and are
familiar with the concepts of components, relations, resources and systems.

## Single-target queries

Let's say you want to implement behavior for regenerating the health of all players of a certain class.
One possible way to design this would be to have a component `Health`, and another for the class, e.g., `Mage`.
Then you could add a system similar to the one below:

```cpp
cubos.system("heal mages").call([](Query<Health&, const Mage&> query) {
    for (auto [health, mage] : query)
    {
        health.points += 1;
    }
});
```

Notice that we don't actually access the mage component at all. We just want it to be present on the entities given by the query.
A better way to write this would be to add an explicit query term:

```cpp
cubos.system("heal mages")
     .with<Mage>() // <--- EXPLICIT TERM
     .call([](Query<Health&> query) {
         for (auto [health] : query)
         {
             health.points += 1;
         }
     });
```

Notice that we could also add `.with<Health>()` above, but that would be redundant.

Explicit query terms bring a lot of possibilities: they can also be used to specify, for example, that entities should *not* have a component.
With this, we can easily prevent healing mages which are already dead:

```cpp
cubos.system("heal alive mages")
     .with<Mage>()
     .without<Dead>()
     .call([](Query<Health&> query) {
        // ...
     });
```

But how do we mark entities as `Dead`? We add the `Dead` component to entities with @ref cubos::core::ecs::Commands::add "Commands::add()", but how do we get the entity handles of the entities with health set to zero? Just adding `Entity` as a query argument type is enough!

```cpp
cubos.system("add dead to entities with zero health")
     .without<Dead>() // no use killing it twice!
     .call([](Commands cmds, Query<Entity, const Health&> query)
     {
        for (auto [entity, health] : query)
        {
            if (health.points <= 0)
            {
                cmds.add(entity, Dead{});
            }
        }
     });
```

## Multiple-target queries

Until now we've only seen queries matching against *single* entities, but what if we want to access relations?
Relations no longer belong to a single entity - and this is where the query system really starts to shine.

Lets add arrows to our fictional RPG. Arrow entities are identified by, you guessed it, an `Arrow` component.
When an arrow hits an entity with health, we want to destroy the arrow and reduce the health.
To check if two entities are colliding, we'll use the @ref cubos::engine::CollidingWith "CollidingWith" relation.

```cpp
cubos.system("arrow damages entity")
     .call([](Commands cmds, Query<Entity, const Arrow&, const CollidingWith&, Health&> query) {
        for (auto [arrowEntity, arrow, collidingWith, health] : query)
        {
            health.points -= 10;
            cmds.destroy(arrowEntity),
        }
     });
```

Notice that once again we're not accessing some of the data: the `Arrow` and `CollidingWith` components.
We can rewrite the query above as:

```cpp
cubos.system("arrow damages entity")
     .entity()
     .with<Arrow>()
     .related<CollidingWith>()
     .with<Health>()
     .call([](Commands cmds, Query<Entity, Health&> query) {
        for (auto [arrowEntity, health] : query)
        {
            health.points -= 10;
            cmds.destroy(arrowEntity),
        }
     });
```

Notice that this time we include `.entity` and `.with<Health>` on the explicit terms to remove the ambiguity that could arise otherwise.
Terms before `.related` are assigned to the target `0`, the relation is assigned to `0, 1` and the terms after it to `1`.
We could also set those targets manually, if we wanted to, by simply passing them as arguments to the terms (e.g. `.with<Health>(1)`).

## Tree Traversal

Tree relations form hierarchies, and the query system provides a way to traverse them in specific BFS order, if necessary.
For example, if you want to traverse the @ref cubos::engine::ChildOf "ChildOf" tree from top to bottom, you could:

```cpp
cubos.system("print all ChildOf relations ordered")
     .entity()
     .related<ChildOf>(Traversal::Up)
     .entity()
     .call([](Query<Entity, Entity> query) {
        for (auto [child, parent] : query)
        {
            CUBOS_INFO("{} is a child of {}", child, parent);
        }
     });
```

The above system would first print all `ChildOf` relations originating from entities which don't have children, then their parents, and so on until reaching the root entities of the hierarchy.
To traverse in the opposite direction you can also use `Traversal::Down`. 

## Pinning

Alright, its cool that we can query over all entities, but what if we just want to do something with the children of specific entity?
You can just pin the parent target to a specific entity!

Notice that the first target of a relation is identified by `0`, the second by `1`, and so on.

```cpp
cubos.system("print all children of some specific entity")
     .entity()
     .related<ChildOf>()
     .call([](Query<Entity> query, const SomeResource& resource) {
        // Forces all matches of the query to have target 1 = resource.entity
        for (auto [child] : query.pin(1, resource.entity))
        {
            CUBOS_INFO("{} is a child of {}", child, entity.entity);
        }
     });
```

To find the parent of an entity, you could also just go the other way around:

```cpp
cubos.system("print the parent of some specific entity")
     .related<ChildOf>()
     .entity()
     .call([](Query<Entity> query, const SomeResource& resource) {
        if (auto match = query.pin(0, resource.entity).first())
        {
            auto [parent] = *match;
            CUBOS_INFO("{} is the parent of {}", resource.entity);
        }
     });
```

## Direct access

If you pin all targets of a query, you get either zero or one matches.
This can be used to access the components of specific entities, or the relation between two specific entities for example.

For example, to access the health of a specific entity, you could:

```cpp
cubos.system("access the health of a specific entity")
     .call([](Query<const Health&> query, const SomeResource& resource) {
        if (auto match = query.pin(0, resource.entity).first())
        {
            auto [health] = *match;
            // Do something with the health component
        }
     });
```

If you wanted to check if two specific entities are colliding, you could:

```cpp
cubos.system("test collisions between specific entities")
     .related<CollidingWith>()
     .call([](Query<> query, const SomeResource& resource) {
        if (query.pin(0, resource.first).pin(1, resource.second).first())
        {
            // They're colliding!
        }
     });
```

Both of the examples can be shortened using the @ref cubos::core::ecs::Query::at "Query::at()" method.
With a single target query, instead of `query.pin(0, entity).first()`, you can write `query.at(entity)`.
With two targets, instead of `query.pin(0, entityA).pin(1, entityB).first()`, you can write `query.at(entityA, entityB)`.
