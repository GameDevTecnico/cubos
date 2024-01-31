# Input {#examples-engine-input}

@brief Using the @ref input-plugin plugin.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/input).

This example shows how the @ref scene-input plugin can be used to handle user input.

The plugin function is included from the @ref engine/input/plugin.hpp header.

@snippet input/main.cpp Adding the plugin

The Input plugin requires a @ref cubos::engine::InputBindings "InputBindings" asset to be set.
Let's take a look at the file the sample uses.

@include assets/sample.bind

There are two types of bindings: `actions` and `axes`.
An `action` is an input that only has two states: pressed or not pressed.
This would be most keys on a keyboard.
An `axe` is an input that has a numeric value.
For example, the joysticks on a controller can go from -1 to 1, depending on how much they are tilt in which direction.
Using `axes` can also be useful for keys with symetric behaviour.
For example, in this sample, `w` sets the `vertical` axe to 1, while `s` sets it to -1.

To define an action or an axe, you simply have to add it to the respective list, giving it a name.
The very first action in the file is called `next-showcase`.
Then, if it's an action, you simply have to define which keys trigger it.
You can also define key combinations by using a `-`.
To check which strings map to which keys, you check the `keyToString` function implementation on [this file](https://github.com/GameDevTecnico/cubos/blob/main/core/src/cubos/core/io/keyboard.cpp). 

Now that we have our bindings file, let's get our application to do something with it.
The first thing we're going to need is a reference to the bindings asset. 
For the purposes of this sample we can simply use an hardcoded reference to the asset.

@snippet input/main.cpp Setting the Bindings

To utilize the bindings, loading them is essential. This can be accomplished by establishing a startup systems which reads from the asset and sets the required bindings.

@snippet input/main.cpp Loading the bindings

Getting the input is done through the @ref cubos::engine::Input resource.
What this sample does is show in order, a series of prompt to showcase the different functionalities of the Input plugin.
For this, it keeps a `state` integer that indicates the current prompt.
Whenever the action `next-showcase` is triggered, it advances to the next prompt.
However, as the plugin currently does not have events, we have to manually check whether the key has just been pressed, is being pressed continuously, or was just released.

@snippet input/main.cpp Checking Type of Press

What this does is only advance the state when the return key is released.
This avoids the state advancing more than once if the user presses it for more than one frame.

Now let's see each of the prompt, to understand the full breadth of the plugin's functionalities.

@note All the functions henceforth are not systems, they are mere functions called by the system above, which passes the `Input` resource to them.

@snippet input/main.cpp Showcase Action Press

Finding out whether the user is pressing a key is checked by a simple call to @ref cubos::engine::Input::pressed "Input::pressed".

@snippet input/main.cpp Showcase Modifier

Getting modified input (such as with a `CTRL` or a `SHIFT` hold) is no different from getting non-modified input, just make sure the binding for it is defined in the Bindings asset.

@snippet input/main.cpp Showcase Multi Modifier

You can have more than one modifier.

@snippet input/main.cpp Showcase Axis

Getting axis is very similar to actions, by calling @ref cubos::engine::Input::axis "Input::axis".
The difference is that this funtion returns a float instead of a boolean value.

@snippet input/main.cpp Showcase Modifier Axis

Modifier keys work with axis too.

@snippet input/main.cpp Showcase Unbound

If, for any reason, you want to read an input that is not defined in the Bindings asset, you cannot use the Input plugin for it.
Instead, you will have to call the @ref cubos::core::io::Window::pressed "Window::pressed" function.

@snippet input/main.cpp Showcase Mouse Buttons

Reading mouse buttons is also supported, just bind them to an action, and then call @ref cubos::engine::Input::pressed "Input::pressed" as usual. To check which strings map to which buttons, you check the `mouseButtonToString` function implementation on [this file](https://github.com/GameDevTecnico/cubos/blob/main/core/src/cubos/core/io/window.cpp).
