# Plugins {#features-plugins}

@brief What are plugins and how they can be used to organize your code.

@m_footernavigation

@note This guide assumes you've read the @ref features-ecs "ECS" guide and are
familiar with the concepts of components, resources and systems.

## Configuring the engine

Plugins are a feature of the @ref cubos::core::ecs::Cubos class. This class is
the main thing you'll be interacting with when developing a game with
**CUBOS.**. It is used to configure the engine and run the game loop. Through
it, you can register new components, resources and configure systems to be run
on specific conditions.

An application which just prints `"Hello World!"` and which does absolutely
nothing else - not even open a window - would look like this:

```cpp
#include <cubos/engine/prelude.hpp>

#include <iostream>

using cubos::engine::Cubos;

void helloWorld()
{
    std::cout << "Hello World!" << std::endl;
}

int main()
{
    Cubos cubos;
    cubos.startupSystem(helloWorld);
    cubos.run();
}
```

## What are plugins?

If you had to add every system, component and resource to your game's main
source file, it would quickly become a mess. Plugins are just functions which
receive a reference to the @ref cubos::engine::Cubos "Cubos" class. Nothing
more.

The idiomatic way to use plugins is through the
@ref cubos::engine::Cubos::addPlugin "Cubos::addPlugin" method. It receives a
plugin function, and executes it only if it hasn't been executed before. This
property is useful for solving dependency issues between plugins. For example,
if `B` and `C` adds `A` and `D` adds `B` and `C`, you would be adding `A`
twice.

If we were to put the hello world functionality on a plugin, the previous
example would look like this:

```cpp
#include <cubos/engine/prelude.hpp>

#include <iostream>

using cubos::engine::Cubos;

void helloWorld()
{
    std::cout << "Hello World!" << std::endl;
}

void helloWorldPlugin(Cubos& cubos)
{
    cubos.startupSystem(helloWorld);
}

int main()
{
    Cubos cubos;
    cubos.addPlugin(helloWorldPlugin);
    cubos.run();
}
```

## What plugins are there?

The engine is basically a big bundle of plugins which add all of kinds of
functionality. You can find an exhaustive list of the plugins provided by the
engine and their documentation on the @ref engine "engine module page". Check
out the @ref examples-engine "engine examples" to see how they are used in
practice.
