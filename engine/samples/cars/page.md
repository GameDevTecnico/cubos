# Cars {#examples-engine-cars}

@brief Loading voxel grids and rendering them.

This example shows how the @ref renderer-plugin plugin and @ref voxels-plugin plugin, can be used to load and render voxel objects.

![](cars/output.png)

Lets start by getting the assets we want to use. Each asset has an id through which we can get a handle to it. This id is specified in the .meta file associated to the asset.

@snippet cars/main.cpp Get handles to assets

Next, we are going to set up the scene. 

We'll need to render and load assets.

@snippet cars/main.cpp Adding the render and voxels plugin

In our assets folder we already have a palette, let's load it and give it some more colors.

@snippet cars/main.cpp Load, add materials and set palette

Now, let's give our scene a floor. To achieve this, we'll create a grid, assign one of the materials we added to the pallete to each voxel of that grid and then spawn it.

@snippet cars/main.cpp Create and spawn floor

To finish the setup, we need to spawn a camera and a light. On this sample we use a directional light. For more info on how to configure the @ref renderer-plugin plugin, refer to the @ref examples-renderer-sample sample.

@snippet cars/main.cpp Spawn camera
@snippet cars/main.cpp Spawn light

In this sample we want to spawn a car every second and move every car in the scene at a constant speed. To do this, we'll use 2 systems. One will spawn the cars and the other will move them.

Let's start with the spawn system. In this system, we'll need to load the asset and spawn the car at a given position, when it's time to do so.

@snippet cars/main.cpp Spawn system

For this to work, we'll need 2 more things, the Car component, which contains the car velocity and angular velocity, and the Spawner resource, used to keep track of the time and the position to spawn the car.

@snippet cars/components.hpp Car component
@snippet cars/main.cpp Spawner resource
@snippet cars/main.cpp Adding component and resource

Lastly, the move system will simply query through all the cars and change the values of their components. Note how we also use the Delta Time resource to make the speed independent from the frame rate.

@snippet cars/main.cpp Move system

Now, to make everything come to life, we just add all of these systems. 

@snippet cars/main.cpp Adding systems

And voilá, you now have a screen full of cars doing ballet!
