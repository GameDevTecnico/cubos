# Structure {#structure}

The source code is divided into three main parts:
- \subpage core - library which is shared between the tools and the games. This
includes some basic functionality like serialization, logging, render devices,
input handling and others.
- \subpage engine - library with code exclusive to the game execution. This includes
the main loop, the asset manager and systems like the renderer and physics.
- \subpage cubinhos - a tool which helps you with the game development and which
may depend on **Core**. One example use case of **Cubinhos** is convert from
external voxel formats to the internal format used by **CUBOS.**.
