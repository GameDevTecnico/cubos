# Introduction {#mainpage}

\image html "CubosLogo.png"

## Voxel based engine

**CUBOS. aims to be a simple, but powerful engine for PC, where everything is**
**made out of voxels.**

It aims to support vast render distances and destructibility, which sets it
apart from other engines. Written in *C++*, built with *OpenGL*, it's a
high-performance engine.

## Structure

The source code is divided into three main parts:
- \ref core - library which is shared between the tools and the games. This
includes some basic functionality like serialization, logging, render devices,
input handling and others.
- \ref engine - library with code exclusive to the game execution. This includes
the main loop, the asset manager and systems like the renderer and physics.
- \ref cubinhos - a tool which helps you with the game development and which
may depend on **Core**. One example use case of **Cubinhos** is convert from
external voxel formats to the internal format used by **CUBOS.**.
