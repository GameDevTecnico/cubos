# Renderer {#examples-engine-renderer}

@brief Using the @ref renderer-plugin plugin.

This example shows how the @ref renderer-plugin plugin can be used and
configured with a simple scene, lighting and split-screen rendering.

![](renderer/output.png)

The plugin function is included from the @ref engine/renderer/plugin.hpp header.
You may need to include other headers, depending on what you want to access.

@snippet renderer/main.cpp Adding the plugin

The first thing we're going to worry about is setting the
@ref cubos::engine::Palette "Palette" the renderer will use. This palette
would usually be loaded from a file, but for this example we'll just create it
manually.

@snippet renderer/main.cpp Setting the palette

We should also spawn a @ref cubos::engine::Grid "voxel grid", so we have
something to render.

@snippet renderer/main.cpp Spawning a voxel grid

If we don't add any lights, the scene will be completely dark. Lets add a
simple @ref cubos::engine::PointLight "PointLight".

@snippet renderer/main.cpp Spawning a point light

We can also add some ambient lighting, and even add a sky gradient, through the
@ref cubos::engine::RendererEnvironment "RendererEnvironment" resource.

@snippet renderer/main.cpp Setting the environment

Lastly, without a camera, we won't be able to see anything. Cameras can be set
using the @ref cubos::engine::ActiveCameras "ActiveCameras" resource.

@snippet renderer/main.cpp Spawn the cameras

Then, its just a matter of adding these systems to the engine. The only one
which requires special attention is the `setPaletteSystem`. This system
accesses the @ref cubos::engine::Renderer "Renderer" resource, which is only
initialized on the tag `cubos.renderer.init`.

@snippet renderer/main.cpp Adding the systems
