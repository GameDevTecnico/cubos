# Exercises {#exercises}

This page contains challenges designed to help you start working with **Cubos**.
We recommend that you start by reading the @ref getting-started "getting started" and @ref features "features" guides.

## Creating a new engine sample

Before moving on to the actual exercises, lets start by creating your own empty sample, which will be expanded upon the next steps.

Start by creating a new folder in the `engine/samples` directory, i.e., `engine/samples/my-own-sample`.
Make sure to add a `main.cpp` to that folder, with the following base code:

```cpp
#include <cubos/engine/prelude.hpp>

using namespace cubos::engine;

int main()
{
      Cubos cubos{};
      cubos.run();
}
```

The final step is to add your new sample to the CMake configuration, at `engine/samples/CMakeLists.txt`.
Open that file, and add a new line of the form `make_sample(DIR "my-own-sample" ASSETS)`.

If you now reconfigure CMake, you should be able to launch the target `engine-sample.my-own-sample`.
Don't worry if nothing shows up yet - in **Cubos** you only get what you ask for, and in the code above we didn't do anything except run an empty application.

## Showing a cube

Try making your sample open a window and show a simple voxel cube! To do this, you will need to:
1. Add the @ref renderer-plugin "Renderer plugin".
2. Create a @ref cubos::engine::VoxelPalette "VoxelPalette".
3. Create a @ref cubos::engine::VoxelGrid "VoxelGrid".
4. Create a @ref cubos::engine::PointLight "PointLight".
5. Create a @ref cubos::engine::Camera "Camera" and add it to @ref cubos::engine::ActiveCameras "ActiveCameras".

You can take inspiration from the @ref examples-engine-hello-cubos "Hello Cubos" @ref examples-engine-renderer "Renderer" samples. 

## Replace the cube by an actual voxel model

Of course we don't expect developers to generate their voxel models through code!
Change your sample to show an actual voxel model, created and stored in a `.qb` file.
You can create one using [MagicaVoxel](https://ephtracy.github.io/), or simply download an existing model from the internet.

**Cubos** doesn't handle `.qb` files directly: it only supports `.grd`'s and `.pal`'s, our own formats for voxel grids and palettes.
You can use **QUADRADOS** to convert a `.qb` to a `.grd` and `.pal`. Check out its @ref features-quadrados "feature guide" for more information.

Take a look at the @ref examples-engine-voxels "Voxels" sample to figure out how you can load those files into the actual sample.

## Move the voxel model on input

Make the model you added previously move when some buttons are pressed. You will need to:
1. Add the @ref input-plugin "Input plugin".
2. Create an input bindings asset.
3. Add a system which uses the @ref cubos::engine::Input "Input" resource to move the model.

Check out the @ref examples-engine-input "Input" sample for inspiration.
