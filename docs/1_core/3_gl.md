# Graphics Library {#core-gl}

The namespace \ref cubos::core::gl provides graphics related utilities, such as
an abstract graphics API (\ref cubos::core::gl::RenderDevice "RenderDevice"),
voxel objects and tools, and other graphics related utilities.

## Render Device

The \ref cubos::core::gl::RenderDevice "RenderDevice" is an abstract interface
for a graphics API. This way, we never use *OpenGL* directly and instead we use
our own graphics API.

This allows us to use different graphics APIs beneath our own, such as
*Vulkan*, *D3D*, *Metal*, *OpenGL ES*, etc, without having to change all of the
rendering code.

At the time of writing, the only
\ref cubos::core::gl::RenderDevice "RenderDevice" implementation uses OpenGL
internally (3.3+). Render devices are never created directly, and instead are
created by the \ref cubos::core::io::Window "io::Window".

You can check the \ref core `render_device.cpp` sample for an example of how to
use the \ref cubos::core::gl::RenderDevice "RenderDevice".

## Palettes and Grids

In **CUBOS.** we use palettes to store information about the types of voxels in
the grids, instead of storing the voxel data directly on the grids. For
example, a voxel object comprised of a single red voxel won't store its color
directly. Instead, it will store the index of a material in the palette which
is red.

This greatly reduces the amount of data we need to store in the grids. Let's
suppose that each voxel material stores RGBA data as four floats (we will
probably add other properties).

Each float occupies 4 bytes, so we need to store 16 bytes per voxel. If we have
a 256x256x256 grid, and store the material directly on the grid, we would need
to store 256^3 * 16 bytes, which is around ~268 MB.

If we, instead, store materials in a palette, and store their indices in the
grids, we can save a lot of space. **CUBOS.** sets a hard-limit of 65535
materials, so two bytes per voxel is enough to identify the material. This
makes our previous grid occupy only ~67 MB.

You can check the \ref cubos::core::gl::Palette "Palette" and
\ref cubos::core::gl::Grid "Grid" classes for more information.

