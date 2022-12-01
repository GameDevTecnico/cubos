# Cubos {#engine-cubos}

Game development revolves around the \ref cubos::engine::Cubos "Cubos" class.
It is used to configure and initialize the engine and the game, and it runs the
main update loop.

\ref cubos::engine::Cubos "Cubos" by itself doesn't do much: it is as barebones
as possible. Internally, it uses two \ref cubos::core::ecs::Dispatcher
"core::ecs::Dispatcher" objects, one for the startup phase and one for the main
loop. It also keeps a \ref cubos::core::ecs::World "core::ecs::World" object,
on which the whole engine and game logic is built.

There are only two resources that are added to the world by default: the
\ref cubos::engine::ShouldQuit "ShouldQuit" resource and the
\ref cubos::core::Settings "core::Settings" resource. The first one is used to
signal the main loop to stop, and the second one is used to store the settings
of the engine and the game.

Custom resource types can be added using the \ref
cubos::engine::Cubos::addResource "Cubos::addResource" method.

No component types are added by default. To register your own, use the \ref
cubos::engine::Cubos::addComponent "Cubos::addComponent" method.
