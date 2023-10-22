# Settings {#examples-engine-settings}

@brief Using the @ref settings-plugin plugin.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/settings).

This example shows how the @ref settings-plugin plugin can be used to load
settings from a file and command-line arguments.

Accesing the settings is straightforward:

@snippet settings/main.cpp System

If the setting `greetings` has been set on the `settings.json` file next to the
sample's executable, or if it has been passed as a command-line argument (e.g.
`./engine-sample.settings --greetings "Hello, world!"`), the sample will output
that value. Otherwise, it will output `Hello!`, which we set as a default.

We want this system to run after the settings have been loaded, so we run it
after the tag `cubos.settings`. Notice that if we want the command-line
arguments to be loaded as settings, we need to pass `argc` and `argv` to the
@ref cubos::engine::Cubos::Cubos(int, char**) "Cubos::Cubos(int, char**)" constructor.

@snippet settings/main.cpp Run
