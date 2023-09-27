# Hello CUBOS. {#examples-engine-hello-cubos}

@brief Using @ref cubos::engine::Cubos "Cubos" to create a simple program.

This example shows the basics of how @ref cubos::engine::Cubos is used, by making a simple "Hello World" program.

@snippet hello-cubos/main.cpp Include

First we'll need to get a `Cubos` object.

@snippet hello-cubos/main.cpp Engine

The @ref cubos::engine::Cubos "Cubos" class represents the engine.
We'll need it to add functionality to our program.

Let's start by defining what functionality we want to add.

@snippet hello-cubos/main.cpp Hello Cubos

This function simply prints `Hello CUBOS` to the console.
It uses one of CUBOS.'s logging macros.
You can find more about them @ref core/include/cubos/core/log.hpp "here".
However, this function is not currently called, so we'll need to tell CUBOS. that we want it to run.

@snippet hello-cubos/main.cpp Set Startup

Startup systems run only once when the engine is loaded.
Now let's make things more interesting.
Let's print `Hello World`, but split it over two different systems.

@snippet hello-cubos/main.cpp Hello World

Instead of using `startupSystem`, we'll use @ref cubos::engine::Cubos::system "Cubos::system".
This means the systems will be called after the startup systems and repeat every cycle, instead of just once at startup.

@note As we don't have anything that would require multiple cycles (such as a window to draw to), the CUBOS. main cycle will run only once.

However, we can't just do as we did for `sayHelloCubos` and call it a day.
We want `sayHello` to come before `sayWorld`, so we'll have to explicitly tell that to the engine, or else we risk having them in the wrong order.
To do that we use tags.
Let's create two tags, one for each system.

@snippet hello-cubos/main.cpp Tags

@ref cubos::engine::Cubos::tag "Cubos::tag" creates a new tag, and @ref cubos::engine::TagBuilder::before "before" makes any systems tagged with it come before systems tagged with the one given as parameter.
There's also an @ref cubos::engine::TagBuilder::after "after" that has the inverse effect.
Now all we have to do is to assign these tags to our systems.
@note If we wanted to give these tags to a system running on startup, we'd have to use @ref cubos::engine::Cubos::startupTag "Cubos::startupTag" instead.

@snippet hello-cubos/main.cpp Set Systems

Now let's see a bit about entities, components and resources.
First we are going to need to use a few more things.
We'll go over what each does as it comes up.

@snippet hello-cubos/main.cpp Include Entity Stuff

Entities have components, so let's create one to give our entities.
Because of how the engine works, we cannot declare a component on our `main.cpp` file.
We'll need to create a `components.hpp` for that.

@include hello-cubos/components.hpp

Here we create a component called "num" that stores a single integer.
We'll use it as the id of the entity it is attached to.
Back on our `main.cpp` file we'll need to include our new file.

@snippet hello-cubos/main.cpp Include Components

And the component needs to be registered.

@snippet hello-cubos/main.cpp Component Add

Let's create a resource now.
Unlike components, resources can be declared on the `main.cpp` file, so let's do that.

@snippet hello-cubos/main.cpp Resource Decl

This resource will store the total number of spawned entities, a population counter of sorts.
It too needs to be registered.

@snippet hello-cubos/main.cpp Resource Add

Now let's create a startup system that spawns some entities.

@snippet hello-cubos/main.cpp Entity Spawn

@ref cubos::core::ecs::Commands "Commands" is a system argument that allows us to interact with the world, in this case, by creating entities that have a `Num` component.

@ref cubos::core::ecs::Write "Write" is a system argument that allows us to modify a resource, in this case `Pop`.

Finally, we'll want a system that prints our entities.

@snippet hello-cubos/main.cpp Entity Print

@ref cubos::core::ecs::Read "Read" is similar to `Write`, only it just gives us permission to read data from resources and components, we cannot alter them.

@ref cubos::core::ecs::Query "Query" allows us to access all entities with a given configuration of components. In this case, it will give us all entities with the `Num` component.

To finish configuring our program, we just need to register these two new systems.
We'll also have the entities be printed after our `Hello World` message.

@snippet hello-cubos/main.cpp Entity System

With everything properly set up, all that remains is to run the engine.

@snippet hello-cubos/main.cpp Run
