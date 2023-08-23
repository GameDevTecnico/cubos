# Input Output {#core-io}

The namespace \ref cubos::core::io provides input output related utilities,
such as a platform independent \ref cubos::core::io::Window "Window" and the
\ref cubos::core::io::InputManager "InputManager".

## Window

The \ref cubos::core::io::Window "Window" class is a handle to a platform
independent window, which can be used to render graphics and receive input
events. It is created by the \ref cubos::core::io::openWindow "openWindow" 
function.

It provides the method \ref cubos::core::io::Window::pollEvents
"Window::pollEvents", which receives an \ref cubos::core::memory::EventWriter
"EventWriter" of \ref cubos::core::io::WindowEvent "WindowEvent"s, polls the
window for events, and writes them to the writer.

## Input Manager

The input manager acts as an abstraction layer between raw input events and
higher level input actions. For example, the game may define an input action
called `"Jump"`, but not care about the actual key that is pressed to trigger
the action. This is where the input manager comes in.

It provides the method \ref cubos::core::io::InputManager::update
"InputManager::update", which receives an \ref cubos::core::memory::EventReader
"EventReader" of \ref cubos::core::io::WindowEvent "WindowEvent"s and an
\ref cubos::core::memory::EventWriter "EventWriter" of
\ref cubos::core::io::InputEvent "InputEvent"s.

### Actions

Actions represent possible high level events that can be triggered by the user.
They are identified by a string, and have an assigned layer. When the action
is triggered and generates an event, the event will have the layer that the
action was assigned to. This allows the game to read only events related to
a specific mask of layers.

Actions thus have two states: *triggered* and *released*. Events are generated
when the state changes, and the event will have the new state.

### Axes

Axes represent a continuous value input. They are identified by a string, and
also have an assigned layer. One example of an axis would be the movement of
the player character. The axis can be positive or negative, and the value
represents the speed of the movement.

Events are generated when the value changes, and the event will have the new
value.

### Bindings

Bindings are what actually trigger actions. There are multiple types of
bindings, such as keyboard key presses, mouse button presses, mouse wheel
scrolls, other actions, axes falling within a range, sequences of other
bindings, etc.

Bindings may also be assigned to axes, in which case they will change the
value of the axis. For example, a binding for the `W` key may be assigned to
the `MoveForward` axis, and when the `W` key is pressed, the axis will have a
positive value.

While actions and axes are defined by the game developer, bindings can be
overridden by the player.

