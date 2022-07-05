# Entity-Component-System {#core-ecs}

[Wikipedia](https://en.wikipedia.org/wiki/Entity_component_system) defines
*ECS* as:

> Entity Component System (ECS) is a software architectural pattern mostly used
> in video game development for the representation of game world objects. An
> ECS comprises entities composed from components of data, with systems which
> operate on entities' components.
> 
> ECS follows the principle of composition over inheritance, meaning that every
> entity is defined not by a type hierarchy, but by the components that are
> associated with it. Systems act globally over all entities which have the
> required components.

The *ECS* thus is a vital part of the engine, since all of the rest is
structured around it in some way. This sets the engine apart from other engines
which follow a more traditional model, like *Unity*, *Unreal* and *Godot*.

## A primer on ECS

### Why are we using this?

*ECS* is a powerful pattern which has been becoming more popular over the
years, and even *Unity* has started integrating it into its engine. The main
advantages are flexibility and performance: it avoids a lot of the problems
that come with traditional object-oriented programming, and regarding
performance, it excels in situations where the number of entities is high,
since it makes use of cache locality. Its also easier to parallelize the
systems, since they have clearly defined dependencies.

### Our implementation

**CUBOS.** *ECS* contains the following concepts:
- **World**: the main object that holds all of the *ECS* state.
- **Entities**: represent objects in the game world.
- **Components**: data associated with an entity (e.g. `Position`, `Rotation`,
`Velocity`, etc.).
- **Resources**: singleton-like objects which are stored per world and which do not belong to a specific entity (e.g. `DeltaTime`, `AssetManager`).
- **Systems**: functions which operate on resources and entities' components.

One important thing to note is that in an *ECS* the data is completely
decoupled from the logic. What this means is that entities and components
do not and should not possess any functionality other than storing data.
All of the logic is relegated to the systems. 

### How do we use it?

Lets say we want to have multiple objects with positions and velocities, and
every frame we want to add their velocities to their positions.

In a traditional object-oriented approach, we would something along the lines of:

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
components:

```cpp
struct Position
{
    // We need to specify how the components of this type are stored.
    using Storage = core::ecs::VecStorage<Position>;
    glm::vec3 vec = { 0.0f, 0.0f, 0.0f };
};

struct Velocity
{
    using Storage = core::ecs::VecStorage<Velocity>;
    glm::vec3 vec = { 0.0f, 0.0f, 0.0f };
};
```

To create a new entity with these components we would do something along the
lines of:

```cpp
auto entity = world->create(Position {}, Velocity {});
```

If we wanted to create a static entity with no velocity, we would just do:
```cpp
auto entity = world->create(Position {});
```

We will also need to define our `DeltaTime` resource:
```cpp
struct DeltaTime
{
    float value = 0.0f;
};
```

So, where does the `update` logic belong? We put it in a system (which is just
a function):

```cpp
void velocitySystem(
    const DeltaTime& dt,
    core::ecs::Query<Position&, const Velocity&> query)
{
    for (auto [entity, position, velocity] : query)
    {
        position.vec += velocity.vec * dt.value;
    }
}
```

In this system we take two arguments: the delta time we want to read and a
query over all entities with both positions and velocities. We request mutable
reference to positions and constant references to velocities, since we only
need to modify the positions.

We can then iterate over all queried entities and update their positions using
their velocities and the delta time.

## Going further

### Registering resources and components

Before components and resources are used in a *World*, they must be registered
on it. This should be done once, at the start of the program. For example, for
the previous example we would need to do:

```cpp
world.registerComponent<Position>();
world.registerComponent<Velocity>();
world.registerResource<DeltaTime>();
```

### Commands

When you have direct access to the *World*, you can manipulate entities directly:
```cpp
auto entity = world.create(Position {}, Velocity {});
world.removeComponent<Velocity>(entity);
world.addComponent(entity, MyComponent {});
world.destroy(entity);
```

What if you want to create, for example, a spawner system which creates new
entities? You're probably thinking that we could just pass the world to the
system. This isn't allowed, since it would give 'too much' power to the system
and we wouldn't be able to determine exactly its requirements and effects.
So, how do we do it?

Until now we've seen systems which operate only on resources and
queries. Systems may also receive a third type of argument: `Commands`.
`Commands` objects store *ECS* commands that are executed at a later time, when
its safe to do so. A possible spawner system could look like this:

```cpp
void spawnerSystem(
    core::ecs::Commands& commands,
    core::ecs::Query<const Spawner&, const Position&> spawners)
{
    for (auto [entity, spawner, position] : spawners)
    {
        commands.create(
            Position { position.vec },
            Velocity { glm::vec3(0.0f, 0.0f, 1.0f )}
        );
        commands.destroy(entity);
    }
}
```

This system iterates over all entities with a `Spawner` component and a
`Position` and creates new entities on their positions with a velocity of
`(0, 0, 1)`, and then destroys the spawner entity. This is safe to do, since
commands are executed only when no system is iterating over the same entities.

### Blueprints

TODO

### Dispatcher

TODO
