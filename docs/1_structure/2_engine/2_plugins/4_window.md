# Window {#engine-plugins-window}

The \ref cubos::engine::plugins::window "window" plugin adds the \ref
cubos::core::io::Window "core::io::Window" resource, and a startup system on
the stage `"window"` which initializes it, by opening the window and creating a
\ref cubos::core::gl::RenderDevice "core::gl::RenderDevice" for it.

The window is created using the settings `window.width`, `window.height`,
`window.title` and `window.vsync`. The default values are `800`, `600`,
`"Cubos Game"` and `true`, respectively.
