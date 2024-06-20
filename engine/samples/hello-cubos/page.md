# Hello Cubos {#examples-engine-hello-cubos}

@brief Using @ref cubos::engine::Cubos "Cubos" to create a simple program.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/hello-cubos).

This example shows the basics of how @ref cubos::engine::Cubos is used, by making a simple "Hello World" program.

@snippet hello-cubos/main.cpp Include

First we'll need to get a `Cubos` object.

@snippet hello-cubos/main.cpp Engine

The @ref cubos::engine::Cubos "Cubos" class represents the engine.
We'll need it to add functionality to our program.

Let's start by defining what functionality we want to add, by adding our first system.

@snippet hello-cubos/main.cpp Hello Cubos

This startup system simply prints `Hello Cubos` to the console, using one of Cubos's logging macros.
You can find more about them @ref core/include/cubos/core/tel/logging.hpp "here".
Startup systems run only once when the engine is loaded.

Now let's make things more interesting.
Let's print `Hello World`, but split it over two different systems.

@snippet hello-cubos/main.cpp Hello World

Instead of using `startupSystem`, we'll use @ref cubos::engine::Cubos::system "Cubos::system".
This means the systems will be called after the startup systems and repeat every cycle, instead of just once at startup.

@note As we don't have anything that would require multiple cycles (such as a window to draw to), the Cubos main cycle will run only once.

Notice that we can't just do as we did for `Hello Cubos` and call it a day.
We want `Hello` to come before `World`, so we'll have to explicitly tell that to the engine, or else we risk having them in the wrong order.
To do that we use tags.

@snippet hello-cubos/main.cpp Tags

@ref cubos::engine::Cubos::tag "Cubos::tag" can be used to apply properties to all systems with a given tag, and @ref cubos::engine::TagBuilder::after "after" makes any systems tagged with it come after systems tagged with the one given as parameter.
There's also an @ref cubos::engine::TagBuilder::before "before" that has the inverse effect.

@note If we wanted to give these tags to a system running on startup, we'd have to use @ref cubos::engine::Cubos::startupTag "Cubos::startupTag" instead.

Now let's see a bit about entities, components and resources.
First we are going to need to use a few more things.
We'll go over what each does as it comes up.

Lets define a new component type, which stores a single integer, which we can use to identify the entity.

@snippet hello-cubos/main.cpp Component Decl

Notice that not only we define the type, but we also define reflection for it. This is a way to let Cubos know what our data type is made of, making it compatible with serialization, UI debug tools and others automatically.

We also need to register the component type with the `Cubos` object.

@snippet hello-cubos/main.cpp Component Add

Now, lets create our own resource.

@snippet hello-cubos/main.cpp Resource Decl

This resource will store the total number of spawned entities, a population counter of sorts.
It too needs to be registered.

@snippet hello-cubos/main.cpp Resource Add

Now let's create a startup system that spawns some entities.

@snippet hello-cubos/main.cpp Entity Spawn

@ref cubos::core::ecs::Commands "Commands" is a system argument that allows us to interact with the world, in this case, by creating entities that have a `Num` component.

To access the resource `Pop`, we just add a reference to it as a system argument (`Pop&`).

Finally, we'll want a system that prints our entities.

@snippet hello-cubos/main.cpp Entity Print

In this case, we don't change `Pop`, and only read its value. Thus, we use `const Pop&`. This allows Cubos to make some optimizations behind the scenes.

@ref cubos::core::ecs::Query "Query" allows us to access all entities with a given configuration of components. In this case, it will give us all entities with the `Num` component.

With everything properly set up, all that remains is to run the engine.

@snippet hello-cubos/main.cpp Run

Try running the sample yourself, and you should see both the hello world messages and the list of entities we spawned!
