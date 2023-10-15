# Gizmos {#examples-engine-gizmos}

@brief Using the @ref gizmos-plugin plugin.

This example shows the @ref gizmos-plugin plugin, which allows drawing simple primitives. These are not intended for use in the final product, only in developer tools and for debugging.

The plugin function is included from the @ref engine/gizmos/plugin.hpp header.

@snippet gizmos/main.cpp Adding plugin

To draw a gizmo, all you need to do is to get a reference to the @ref cubos::engine::Gizmos resource, and then call the draw function on it for the gizmo you want to draw. Additionally, you can also call the @ref cubos::engine::Gizmos::color function to set the color for future gizmos. So, for example if you want to draw a line in a given system, all you need to do is the following:

@snippet gizmos/main.cpp Start Up System

This code will draw a red line from the top-left of the screen to the bottom right, and it will stay there for 10 seconds.

In this other example, we draw a line, a box, and a wire box. Unlike the one in the previous example, this system is not a start-up system, so the draw functions get called every single frame. When this happensl, you should set the lifetime of a gizmo to 0, which means it will be drawn for a single frame only. This way we avoid drawing gizmos on top of identical ones that were already there, or in the case of moving gizmos, leaving a trail of old version behind them.

@snippet gizmos/main.cpp System