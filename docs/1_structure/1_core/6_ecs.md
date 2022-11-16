# ECS {#core-ecs}

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
- **Entities**: represent objects in the game world (e.g. a car, a player, a tree)
- **Components**: data associated with an entity (e.g. `Position`, `Rotation`,
`Velocity`, etc.).
- **Resources**: singleton-like objects which are stored per world and which do not belong to a specific entity (e.g. `DeltaTime`, `AssetManager`).
- **Systems**: functions which operate on resources and entities' components. This is where the logic is implemented.
- **Dispatcher**: decides when each system is called and knows which systems are independent from each other so they can be called at the same time (parallel computing).

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
struct [[cubos::component("position", VecStorage)]] Position
{
    glm::vec3 vec = { 0.0f, 0.0f, 0.0f };
};

struct [[cubos::component("velocity", VecStorage)]] Velocity
{
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
`Commands` objects store *ECS* commands to be executed at a later time, when
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

A common pattern in game engines is to have a way to create entities in
bundles, such as *Unity*'s *prefabs*. In **CUBOS.** we call these *blueprints*.
A blueprint is a set of entities and their components, which can be spawned
into the world as many times as needed.

For example, if we wanted to create a blueprint for a motorbike with two
wheels:
```cpp
auto motorbike = ecs::Blueprint();
auto body = motorbike.create("body", Position{...}, ...);
motorbike.create("front_wheel", Parent{body}, ...);
motorbike.create("back_wheel", Parent{body}, ...);
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
wheel.create("wheel", ...);
```

When creating the motorbike blueprint, we can merge the wheel blueprint into
it:
```cpp
auto motorbike = ecs::Blueprint();
auto body = motorbike.create("body", Position{...}, ...);
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

### Dispatcher

The \ref cubos::core::ecs::Dispatcher "Dispatcher" is the object which manages
when each system is called. It is used to organize the order in which systems
are called each frame (or at startup) and to organize the systems in a way to
know which systems do not share resources between them, as in, don't write and
read from the same component or resource at the same time) so they can be
called in parallel.

#### How is it organized?

The dispatcher keeps a list of **stages**, each stage represents a step in
which its systems are called (possibly in parallel). Each stage has a list of
**systems**. Since these systems may be called in parallel, they must not
share resources between them.

#### Adding systems to the dispatcher

When a system is added to the dispatcher, to a stage which was not yet
created, the \ref cubos::core::ecs::Dispatcher "Dispatcher" will create a new
stage and put it in the default position.

#### Stage ordering

If you want to specify a certain order for the stages, you can use the
\ref cubos::core::ecs::Dispatcher::putStageBefore "Dispatcher::putStageBefore"
and
\ref cubos::core::ecs::Dispatcher::putStageAfter "Dispatcher::putStageAfter"
methods. These move a stage to a certain position in the list of stages, in
relation to another stage.

You can also set the default position for new stages using the
\ref cubos::core::ecs::Dispatcher::setDefaultStage
"Dispatcher::setDefaultStage".
