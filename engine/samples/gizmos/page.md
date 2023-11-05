# Gizmos {#examples-engine-gizmos}

@brief Using the @ref gizmos-plugin plugin.

This example shows the @ref gizmos-plugin plugin, which allows drawing simple primitives. These are not intended for use in the final product, only in developer tools and for debugging.

The plugin function is included from the @ref engine/gizmos/plugin.hpp header.

@snippet gizmos/main.cpp Adding plugin

To draw a gizmo, all you need to do is to get a reference to the @ref cubos::engine::Gizmos resource, and then call the draw function on it for the gizmo you want to draw. Additionally, you can also call the @ref cubos::engine::Gizmos::color function to set the color for future gizmos. So, for example if you want to draw an arrow in a given system, all you need to do is the following:

@snippet gizmos/main.cpp Start Up System

This code will draw an arrow poiting at the center of the screen, and it will stay there for 10 seconds.

In this other example, we draw lines, a box, and a wire box. Unlike the one in the previous example, this system is not a start-up system, so the draw functions get called every single frame. When this happens, you should set the lifetime of a gizmo to 0, which means it will be drawn for a single frame only. This way we avoid drawing gizmos on top of identical ones that were already there, or in the case of moving gizmos, leaving a trail of old version behind them.

Let's start with the lines. We are using four cameras in our scene, so let's add two lines to separate each camera. These lines will be in Screen space, as we want them to be just drawn once, indepently of the number of cameras; and we want them to use screen coordinates, as they should be drawn in the middle of the screen.

@snippet gizmos/main.cpp Lines

Let's now add a wireboxe. We want to know were exactly is the centre of each camera, so let's add a reticule. This box will be in View space, as we want it to be drawn once per camera and we want it to use view coordinates, as it should be drawn in the middle of each viewport.

@snippet gizmos/main.cpp WireBox

Let's add a box. This box will be in World space, as it's the last space left to cover. It will be drawn by any camera that is looking at it, much like if it was an object in the world.

@snippet gizmos/main.cpp Box

Finally let's add a cut cone. A cut cone is cylinder with faces that can have different radiuses. If you set one of the bases to have a radius of 0, you'll have a simple cone. If you set them both to have the same radius, you'll have a cylinder. Our cut cone will have different radiuses:

@snippet gizmos/main.cpp Cut Cone

For the cut cone, we'll set the color a bit differently: We'll make it so the color of the cone changes depending on whether you are pressing the cone, or have your mouse over it. We'll make it a bit darker while the mouse is not over the cone, a bit lighter when it is, and even lighter when the cone is pressed.

The whole system looks like this:

@snippet gizmos/main.cpp System
