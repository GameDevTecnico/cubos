# Voxels {#examples-engine-voxels}

@brief Using the @ref voxels-plugin plugin.

This example shows how the @ref voxels-plugin plugin, can be used to load voxel grids.

![](voxels/output.png)

The plugin function is included from the @ref engine/voxels/plugin.hpp header.
To see the asset, you'll also need to perform a basic configuration of the @ref renderer-plugin plugin like shown in the @ref examples-renderer-sample sample.

@snippet voxels/main.cpp Adding the plugin

Lets start by getting the assets we want to use. Each asset has an id through which we can get a handle to it. This id is specified in the .meta file associated to the asset.

@snippet voxels/main.cpp Get handles to assets

Next, let's assign the palette, that we have in our assets folder, to be used by the renderer.

@snippet voxels/main.cpp Load and set palette

Now, we can create an entity with our car asset. 

@snippet voxels/main.cpp Spawn car system

Finally, we just add these systems. 

@snippet voxels/main.cpp Adding systems

And voilá, you now have a car floating in space.