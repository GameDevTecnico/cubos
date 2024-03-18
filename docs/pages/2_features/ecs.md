# ECS {#features-ecs}

@brief What is an ECS and how it's used in **CUBOS.**

@m_footernavigation

[Wikipedia](https://en.wikipedia.org/wiki/Entity_component_system) defines
ECS as:

> Entity Component System (ECS) is a software architectural pattern mostly used
> in video game development for the representation of game world objects. An
> ECS comprises entities composed from components of data, with systems which
> operate on entities' components.
> 
> ECS follows the principle of composition over inheritance, meaning that every
> entity is defined not by a type hierarchy, but by the components that are
> associated with it. Systems act globally over all entities which have the
> required components.

The ECS thus is a vital part of the engine, since all of the rest is
structured around it in some way. This sets the engine apart from other engines
which follow a more traditional model, like *Unity*, *Unreal* and *Godot*.

## A primer on ECS

### Why are we using this?

ECS is a powerful pattern which has been becoming more popular over the
years, and even *Unity* has started integrating it into its engine. The main
advantages are flexibility and performance: it avoids a lot of the problems
that come with traditional object-oriented programming, and regarding
performance, it excels in situations where the number of entities is high,
since it makes use of cache locality. Its also easier to parallelize the
systems, since they have clearly defined dependencies.

### Our implementation

**CUBOS.** ECS contains the following concepts:
- **Cubos** - used to configure and run an ECS world and systems.
- **World** - the main object that holds all of the ECS state.
- **Entities** - represent objects in the game world (e.g. a car, a player, a 
  tree).
- **Components** - data associated with an entity (e.g. @ref
  cubos::engine::Position "Position", @ref cubos::engine::Rotation "Rotation").
- **Relations** - data associated to a pair of entities (e.g., @ref cubos::engine::ChildOf "ChildOf",
  @ref cubos::engine::CollidingWith "CollidingWith")
- **Resources** - singleton-like objects which are stored per world and which
  do not belong to a specific entity (e.g. @ref cubos::engine::DeltaTime
  "DeltaTime", @ref cubos::engine::Input "Input").
- **Systems** - functions which operate on resources and entities' components.
  This is where the logic is implemented.
- **Observers** - basically systems which are called immediately after some event is triggered.

One important thing to note is that in an ECS the data is completely
decoupled from the logic. What this means is that entities and components
do not and should not possess any functionality other than storing data.
All of the logic is relegated to the systems.

### How do we use it?

Lets say we want to have multiple objects with positions and velocities, and
every frame we want to add their velocities to their positions.

In a traditional object-oriented approach, we would have something along the
lines of:

```cpp
class MyObject : public GameObject
{
public:
    // ...

    virtual void update(float deltaTime) override
    {
        this->position += this->velocity * deltaTime;
    }

private:
    glm::vec3 position;
    glm::vec3 velocity;
}
```

In **CUBOS.** there are no game objects. Instead, we would define two
components and a `DeltaTime` resource:

```cpp
struct Position
{
    CUBOS_REFLECT;

    glm::vec3 vec = { 0.0f, 0.0f, 0.0f };
};

struct Velocity
{
    CUBOS_REFLECT;

    glm::vec3 vec = { 0.0f, 0.0f, 0.0f };
};

struct DeltaTime
{
    float value = 0.0f;
};
```

So, where does the `update` logic belong? We put it in a system. Systems are
functions whose arguments are of certain types, which you can read more about
in the @ref core-ecs "ECS module page".

To access a resource, simply add a reference to it as an argument. In this case,
we won't be modifying the delta time, so we will use `const DeltaTime&`. To
access entities and their components, we use the `Query` argument. In this case
we we want to access all entities with both positions and velocities, and
modify their positions, so we will use `Query<Position&, const Velocity&>`.

```cpp
cubos.system("integrate velocity")
     .call([](const DeltaTime& dt, Query<Position&, const Velocity&> query) 
     {
         for (auto [position, velocity] : query)
         {
             position->vec += velocity->vec * dt->value;
         }
     });
```

We can then iterate over all queried entities and update their positions using
their velocities and the delta time.

## Going further

### Registering types

Before components, relations and resources are used in a World, they must be registered
on it. This should be done once, at the start of the program. For example,
using the **CUBOS.** @ref cubos::core::ecs::Cubos "main class", for the
previous example we would write:

```cpp
cubos.component<Position>();
cubos.relation<ChildOf>();
cubos.resource<DeltaTime>();
```

@note If you're working directly with a World, you can use methods such as
@ref cubos::core::ecs::World::registerComponent() "World::registerComponent()" to achieve the same result.

### Commands

When you have direct access to the World, you can manipulate entities directly:

```cpp
auto entity = world.create();
world.components(entity).add<Position>();
world.components(entity).remove<Position>();
world.components(entity).add<Dead>();
world.destroy(entity);
```

If necessary, you can access the world in a system through the arguments
`const World&` or `World&`. However, this is not recommended, since it
becomes impossible to know what the system is accessing, and thus we cannot
parallelize it.

Instead, you should use the @ref cubos::core::ecs::Commands "Commands"
argument. Through it you can queue operations to be executed at a later time,
when its safe to do so.

Imagine we want to have spawners which create new entities on their position
and then destroy themselves. We can implement this with a system like this:

```cpp
cubos.system("spawn entities")
     .call([](Commands commands, Query<Entity, const Spawner&, const Position&> spawners)
     {
         for (auto [entity, spawner, position] : spawners)
         {
             commands.create()
                     .add(position)
                     .add(Velocity{{0.0F, 0.0F, 1.0F}});
             commands.destroy(entity);
         }
     });
```

This system iterates over all entities with `Spawner` and `Position`
components. It creates new entities on their positions with a velocity of
`(0, 0, 1)`, and then destroys the spawner entity. The entities are only
actually created and destroyed later on, when the commands are executed.

### Observers

If you find yourself wanting to apply some logic whenever a given component is added or removed, observers are the right tool for you!
For example, one use case that arises frequently when working with CUBOS, is when you have multiple components that must be added to an entity for it perform some function correctly.
This is the case for the @ref cubos::engine::Position "Position" component. If the entity doesn't have a @ref cubos::engine::LocalToWorld "LocalToWorld" component, then it's meaningless.
To solve this, we do something along the lines of:

```cpp
cubos.observer("add LocalToWorld to positions")
     .onAdd<Position>()
     .without<LocalToWorld>()
     .call([](Commands cmds, Query<Entity> query) {
        for (auto [ent] : query)
        {
            cmds.add(ent, LocalToWorld{});
        }
     });
```

### Relations

Relations are just like components, but they're much more powerful.
Instead of being associated to a single entity, they're associated to two entities: the 'from' entity and the 'to' entity.
There are three types of relations:
- Default - relations which have a orientation (e.g. from and to have distinct meanings), and which have no restriction on the number.
- Tree - relations which too have orientation, but with the extra restriction that an entity can only appear in the 'from' side of at least one instance of the relation type. One example is the @ref cubos::engine::ChildOf "ChildOf" relation.
- Symmetric - relations whose orientation doesn't matter. One example is the @ref cubos::engine::CollidingWith "CollidingWith" relation.

They can be created using either @ref cubos::engine::Commands::relate "Commands::relate()" or the @ref cubos::core::ecs::World::relate "World::relate()" methods. They can also be destroyed using @ref cubos::engine::Commands::unrelate "Commands::unrelate()" or @ref cubos::core::ecs::World::unrelate "World::unrelate()".

Relations can also be queried in systems - read the multiple-target section on the @ref features-queries guide for more information on that.

### Blueprints

A common pattern in game engines is to have a way to create entities in
bundles, such as *Unity*'s *prefabs*. In **CUBOS.** we call these *blueprints*.
A blueprint is a set of entities and their components, which can be spawned
into the world as many times as needed.

For example, if we wanted to create a blueprint for a motorbike with two
wheels:
```cpp
auto motorbike = ecs::Blueprint();

auto body = motorbike.create("body");
motorbike.add(body, Position{...});

auto frontWheel = motorbike.create("front_wheel");
motorbike.add(frontWheel, Position{...});
motorbike.relate(frontWheel, body, ChildOf{});

auto backWheel = motorbike.create("back_wheel");
motorbike.add(backWheel, Position{...});
motorbike.relate(backWheel, body, ChildOf{});
```

To spawn the blueprint into the world:

```cpp
// You can just spawn the bike as is.
commands.spawn(motorbike);

// Lets say we want to spawn the bike in a different position.
// You can override components of the spawned entities.
commands
    .spawn(motorbike)
    .add("body", Position{...});
```

#### Merging blueprints

It's possible to merge a blueprint into another one. Lets say we have a
blueprint for the wheels of the motorbike:

```cpp
auto wheel = ecs::Blueprint();
wheel.create("wheel");
```

When creating the motorbike blueprint, we can merge the wheel blueprint into
it:
```cpp
auto motorbike = ecs::Blueprint();
auto body = motorbike.create("body", Position{...});
motorbike.merge("front", wheel);
motorbike.merge("back", wheel);
```

When spawning the motorbike blueprint, the entities of the wheel blueprint
will be accessible with the prefix we gave them:
```cpp
commands
    .spawn(motorbike)
    .add("front.wheel", ...)
    .add("back.wheel", ...);
```
