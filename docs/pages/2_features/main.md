# Feature guide {#features}

@brief High-level introduction to design of the **CUBOS.** engine and its
features.

Before you continue further, here's a quick overview of the engine's design.

## Overview

The **CUBOS.** engine is split into two libraries: @ref core and @ref engine.
Each is on their own namespace, which corresponds to a sub-folder in the
include path.

@ref core provides platform abstraction, containers and utilities, as well as
the @ref core-ecs "ECS" module. @ref engine builds on top of @ref core and
provides higher-level features, such as rendering and collisions.

## Learn more

Each of the following pages provides a high-level description of a feature of
the engine. It's recommended to read through these first before diving into
documentation of each class and function.

- @subpage features-ecs - @copybrief features-ecs
- @subpage features-plugins - @copybrief features-plugins
- @subpage features-quadrados - @copybrief features-quadrados

## What we don't have and where are we going

The engine is still in its early stages of development, and thus we're missing
a lot of features. For example, we don't have physics yet, and the editor tools
are very barebones - you can't even load a scene from a file yet. There's no
scripting language, thus you will have to write all your game logic in C++.

Making a game with the engine is still very painful. Our goal is to make
**CUBOS.** usable for game jams by people who are not on the team. This means a
lot of work, but we're confident we can get there.
