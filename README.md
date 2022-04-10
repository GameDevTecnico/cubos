# **CUBOS.** Engine

<p align="center">
  <!-- if we ever get a site, we can put the link here-->
  <!-- <a href="https://godotengine.org"> -->
    <img src="docs/images/CubosLogo.png" alt="cubos. Engine logo">
  <!-- </a> -->
</p>

## Voxel based engine

**CUBOS. is a simple, but powerful engine for PC, where everything is made out of voxels.**

**IMPORTANT NOTE:** **CUBOS.** is a work in progress still in its very early stages. This description matches our hopes for the engine, not its current state.

**CUBOS.** is a 3D game engine that is perfect for beginners, but also a powerful tool in the hands of experienced developers.
Its vast render distances and the destructibility it allows set it apart from other engines.
Written in *C++*, built with *OpenGL*, it's a high-performance engine.

## Structure

The source code is divided into three main parts:
- `core`: library which is shared between the tools and the games. This includes some basic functionality like serialization, logging, render devices, input handling and others.
- `engine`: library with code exclusive to the game execution. This includes the main loop, the asset manager and systems like the renderer and physics.
- `tools`: contains programs that help you with the game development and which may depend on the `core`. One example is the future `editor`. Right now the only finished tool is the `embed` tool.

### Further reading

You can find more information about how the engine is structured in the [documentation](https://gamedevtecnico.github.io/cubos/).

## Getting the engine

### Executables

There are no releases of **CUBOS.** so far. We will update this section once we release official binaries.
<!--Official binaries for **CUBOS.** can be found
on the [releases](https://github.com/GameDevTecnico/cubos/releases) page.-->

### Building from the source code

#### Dependencies

The following dependencies are used to compile **CUBOS.**:

| Name                                               | Importance         | Submodule Path        | Installed Separately |
| -------------------------------------------------- | ------------------ | --------------------- | -------------------- |
| [CMake](https://cmake.org/)                        | Essential          | -                     | Yes                  |
| [glad](https://github.com/Dav1dde/glad)            | Essential          | -                     | No                   |
| [glfw](https://github.com/glfw/glfw)               | Essential          | `core/lib/glfw`       | Optionally           |
| [glm](https://github.com/g-truc/glm)               | Essential          | `core/lib/glm`        | Optionally           |
| [fmt](https://github.com/fmtlib/fmt)               | Essential          | `core/lib/fmt`        | Optionally           |
| [spdlog](https://github.com/gabime/spdlog)         | Essential          | `core/lib/spdlog`     | Optionally           |
| [yaml-cpp](https://github.com/jbeder/yaml-cpp)     | Essential          | `core/lib/yaml-cpp`   | Optionally           |
| [googletest](https://github.com/google/googletest) | Required for tests | `core/lib/googletest` | Optionally           |

Dependencies marked as *Essential* are required to compile the engine.
**CUBOS.** uses [CMake](https://cmake.org/) as its build system, so you must install it to compile the engine.

Most dependencies come as submodules, so, you may choose whether to install them separately or not. If you choose to install some or all of them separately, you can do so easily by using the package manager of your choice. If you use Windows (ew), it should be easier to simply install all of them with the engine by cloning
the whole repository with the `--recursive` flag.

#### Compiling

If you chose to install some dependencies separately, you must pass the corresponding `-D` flag to CMake. For example, if you installed GLFW, GLM and CMake separetely you would pass the following flags to CMake:

`cmake -H. -Bbuild -DGLFW_USE_SUBMODULE=OFF -DGLM_USE_SUBMODULE=OFF -DYAMLCPP_USE_SUBMODULE=OFF`

The following is a list of all the options available to configure the engine:

| Name                       | Description                        |
| -------------------------- | ---------------------------------- |
| `WITH_GLFW`                | Use GLFW? (Required for now)       |
| `WITH_OPENGL`              | Use OpenGL? (Required for now)     |
| `GLFW_USE_SUBMODULE`       | Compile glfw from source?          |
| `GLM_USE_SUBMODULE`        | Compile glm from source?           |
| `YAMLCPP_USE_SUBMODULE`    | Compile yaml-cpp from source?      |
| `GOOGLETEST_USE_SUBMODULE` | Compile GoogleTest from source?    |
| `SPDLOG_USE_SUBMODULE`     | Compile spdlog from source?        |
| `FMT_USE_SUBMODULE`        | Compile fmt from source?           |
| `BUILD_CORE_SAMPLES`       | Build **CUBOS.** `core` samples?   |
| `BUILD_CORE_TESTS`         | Build **CUBOS.** `core` tests?     |
| `BUILD_ENGINE_SAMPLES`     | Build **CUBOS.** `engine` samples? |

### Samples

Both the `core` and the `engine` contain samples that you can run to get an idea of how the engine works.

### Testing

**CUBOS.** uses GoogleTest for unit testing the engine.
To test the engine's core you can use the following command: `cd build/core && ctest`.

## Who is making this engine

We are  [Gamedev Técnico](https://www.instagram.com/gamedevtecnico/), a student group from [Instituto Superior Técnico](https://tecnico.ulisboa.pt/en/) who makes games. Our goal is to build a game engine from the ground up. 

Find us at:

[Twitter](https://twitter.com/GameDevTecnico)

[Facebook](https://www.facebook.com/Game-Dev-T%C3%A9cnico-107405047487324/)

[Instagram](https://www.instagram.com/gamedevtecnico/)

[Youtube](https://www.youtube.com/channel/UCpJf5Ih7SE9wAgaZ_OF9qYA)

[itch.io](https://gamedevtecnico.itch.io/)

