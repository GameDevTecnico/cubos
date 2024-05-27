# Plugins {#features-plugins}

@brief What are plugins and how they can be used to organize your code.

@m_footernavigation

@note This guide assumes you've read the @ref features-ecs "ECS" guide and are
familiar with the concepts of components, resources and systems.

## Configuring the engine

Plugins are a feature of the @ref cubos::core::ecs::Cubos class. This class is
the main thing you'll be interacting with when developing a game with
**Cubos**. It is used to configure the engine and run the game loop. Through
it, you can register new components, relations, resources and configure systems to be run on specific conditions.

Below is an example of an application which:
1. adds a resource with the string `"Hello World"`.
2. prints its contents.

```cpp
#include <cubos/engine/prelude.hpp>

#include <iostream>

using cubos::engine::Cubos;

struct Message
{
    std::string value = "Hello World!";
};

int main()
{
    Cubos cubos;

    cubos.resource<Message>();

    cubos.startupSystem("show message").call([](const Message& msg) {
        std::cout << msg.value << std::endl;
    });

    cubos.run();
}
```

## What are plugins?

If you had to add every system, component and resource to your game's main
source file, it would quickly become a mess. Plugins are just functions which
receive a reference to the @ref cubos::engine::Cubos "Cubos" class. Nothing
more.

The idiomatic way to use plugins is through the @ref cubos::engine::Cubos::plugin "Cubos::plugin" and @ref cubos::engine::Cubos::depends "Cubos::depends" methods. The first receives a function to a plugin, and executes it. The second marks the current plugin as depending on the given plugin.

Lets take the example above and move the functionality to a plugin.

```cpp
// ...

void helloWorldPlugin(Cubos& cubos)
{
    cubos.resource<Message>(Message{"Hello World!"});

    cubos.startupSystem("show message").call([](const Message& msg) {
        std::cout << msg.value << std::endl;
    });
}

int main()
{
    Cubos cubos;

    cubos.plugin(helloWorldPlugin);

    cubos.run();
}
```

Looks good! But lets pretend for a moment that there's a lot of logic necessary to create the message and resource.
How could we divide this further?

```cpp
void messagePlugin(Cubos& cubos)
{
    cubos.resource<Message>(Message{"Hello World!"});
}

void showPlugin(Cubos& cubos)
{
    cubos.depends(messagePlugin);

    cubos.startupSystem("show message").call([](const Message& msg) {
        std::cout << msg.value << std::endl;
    });
}

int main()
{
    Cubos cubos;

    cubos.plugin(messagePlugin);
    cubos.plugin(showPlugin);

    cubos.run();
}
```

Notice that the `showPlugin` marks itself as depending on `messagePlugin`. This is necessary, as we use a resource which is defined in that plugin. Since `showPlugin` depends on `messagePlugin`, we must add it before the `showPlugin`.

The two main rules to be take into account are:
- plugins may only be added once (through @ref cubos::engine::Cubos::plugin "Cubos::plugin").
- plugins cannot use tags, resources, components or relations from plugins they didn't add or don't depend on.

## What plugins are there?

The engine is basically a big bundle of plugins which add all of kinds of
functionality. You can find an exhaustive list of the plugins provided by the
engine and their documentation on the @ref engine "engine module page". Check
out the @ref examples-engine "engine examples" to see how they are used in
practice.
