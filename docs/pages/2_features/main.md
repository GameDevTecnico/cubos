# Feature Guide {#features}

This is a brief introduction to the **Cubos** engine and its features. Before
diving into the documentation of each class and function, it's recommended to
read through this guide.

## Overview

The **Cubos** engine is divided into two libraries: @ref core and @ref engine.
Each library has its own namespace, which corresponds to a sub-folder in the
include path.

@ref core provides platform abstraction, containers, utilities, and the
@ref core-ecs "ECS" module. @ref engine builds on top of @ref core and provides
higher-level features such as rendering and collisions.

The core library is divided into modules, each of which covers a specific area
of functionality. For example, graphics-related functionality is on the
@ref cubos::core::gl namespace, under the include path `cubos/core/gl`.

The engine library is divided into plugins, each of which extends the engine's
functionality in some way. You'll learn more about plugins below. For each
plugin, there is a corresponding directory in the include path. Categories of
plugins are also divided into sub-folders, with their own namespaces.

### Tooling

Other than the core and engine libraries, the project also contains *Quadrados*
and *Tesseratos*. *Quadrados* is a CLI tool which contains utilities used
during game development. *Tesseratos* is our in-house editor, which is still
in its early stages of development.

## Learn more

The following guides explain basic concepts of the engine. Its recommend to
read through these first to understand the overall principles and only then
dive into the documentation of each class and function.

- @subpage features-ecs - @copybrief features-ecs
- @subpage features-plugins - @copybrief features-plugins
- @subpage features-queries - @copybrief features-queries
- @subpage features-quadrados - @copybrief features-quadrados

## What we don't have and where are we going

The engine is still in its early stages of development, and thus we're missing
a lot of features. For example, we don't have rigid body physics yet, and the editor tools are very bare bones.
There's no scripting language, thus you will have to write all your game logic in C++.

Making a game with the engine is still very painful. Our goal is to make
**Cubos** usable for game jams by people who are not on the team. This means a
lot of work, but we're confident we can get there.
