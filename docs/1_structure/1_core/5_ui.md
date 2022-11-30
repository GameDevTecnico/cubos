# User Interface {#core-ui}

The namespace \ref cubos::core::ui provides UI related utilities. **CUBOS.** UI
support is built on top of [*Dear ImGui*](https://github.com/ocornut/imgui).

## ImGui integration

To start using *ImGui* on a **CUBOS.** window, you must first call the
\ref cubos::core::ui::initialize "initialize" function, passing the window
handle. This will initialize *ImGui* and create the necessary resources.

At the end of the program, you should call the
\ref cubos::core::ui::terminate "terminate" function to release the resources.

For each frame, you must call the \ref cubos::core::ui::beginFrame "beginFrame"
function before any *ImGui* calls, and the \ref cubos::core::ui::endFrame
"endFrame" function after all *ImGui* calls, to render the UI.

## Showing objects on the UI

The \ref cubos::core::ui::show "show" function can be used to show any
serializable object on the UI. It will show the object's properties on a
*ImGui* table. There's also the \ref cubos::core::ui::edit "edit" function,
which is similar to \ref cubos::core::ui::show "show", but allows editing the
object's properties, in case it is deserializable.
