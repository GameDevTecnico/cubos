# Getting started {#getting-started}

@brief How to download, build and run the engine and where to go from there.

## Building the engine from source

@note This guide assumes you're familiar with the basics of git.

### Getting the code

The **CUBOS.** source code is [hosted on GitHub](https://github.com/GameDevTecnico/cubos),
so the easiest and only way to get the code right now is to clone the
repository with git.

### Dependencies

The following dependencies are used to compile **CUBOS.**:

| Name                                                | Importance                | Submodule Path     | Installed Separately | Version* |
| --------------------------------------------------- | ------------------------- | ------------------ | -------------------- | -------- |
| [CMake](https://cmake.org/)                         | Essential                 | -                  | Yes                  | -        |
| [glad](https://github.com/Dav1dde/glad)             | Essential                 | `core/lib/glad`    | No                   | 2.0.4    |
| [glfw](https://github.com/glfw/glfw)                | Essential                 | `core/lib/glfw`    | Optionally           | 3.3.8    |
| [glm](https://github.com/g-truc/glm)                | Essential                 | `core/lib/glm`     | Optionally           | 0.9.9.8  |
| [stduuid](https://github.com/mariusbancila/stduuid) | Essential                 | `core/lib/stduuid` | No                   | 1.2.3    |
| [doctest](https://github.com/doctest/doctest)       | Required for tests        | `core/lib/doctest` | Optionally           | 2.4.11   |
| [imgui](https://github.com/ocornut/imgui)           | Required for imgui plugin | `engine/lib/imgui` | No                   | 1.89.9   |
| [json](https://github.com/nlohmann/json)            | Essential                 | `core/lib/json`    | No                   | 3.11.2   |

*versions tested in CI and installed by submodules, others might work

Dependencies marked as *Essential* are required to compile the engine. If they
are installed separately, you must first install them manually. If they're not,
it means they come as submodules and you can install them with the engine by
cloning the whole repository with the `--recursive` flag. If you've already
cloned the repository, you can install them by running the following command
from the root directory of the repository:

```bash
git submodule update --init --recursive
```

### Formatting

To contribute you must ensure your code is correctly formatted. Install
`clang-format` and run the following command to enable a pre-commit hook
which formats your changes automatically:

```bash
git config --local include.path ../.gitconfig
```

### Compiling

If you chose to install some dependencies separately, you must pass the
corresponding `-D` flag to CMake. For example, if you installed GLFW, GLM and
CMake separately you would pass the following flags to CMake:

```bash
cmake -H. -Bbuild -DGLFW_USE_SUBMODULE=OFF -DGLM_USE_SUBMODULE=OFF
```

If you are using submodules, you can simply run the following command from the
root directory of the repository:

```bash
cmake -H. -Bbuild
```

The following is a list of all the options available to configure the engine:

| Name                    | Description                          |
| ----------------------- | ------------------------------------ |
| `WITH_GLFW`             | Use GLFW? (Required for now)         |
| `WITH_OPENGL`           | Use OpenGL? (Required for now)       |
| `GLFW_USE_SUBMODULE`    | Compile glfw from source?            |
| `GLM_USE_SUBMODULE`     | Compile glm from source?             |
| `DOCTEST_USE_SUBMODULE` | Compile doctest from source?         |
| `BUILD_CORE_SAMPLES`    | Build **CUBOS.** `core` samples?     |
| `BUILD_CORE_TESTS`      | Build **CUBOS.** `core` tests?       |
| `BUILD_ENGINE_SAMPLES`  | Build **CUBOS.** `engine` samples?   |
| `BUILD_ENGINE_TESTS`    | Build **CUBOS.** `engine` tests?     |
| `BUILD_DOCUMENTATION`   | Build the documentation?             |
| `ENABLE_COVERAGE`       | Enable code coverage? (GCC only)     |
| `FIX_CLANG_TIDY_ERRORS` | Fix clang-tidy errors automatically? |

## Running the examples and tests

### Examples

Both the `core` and the `engine` contain examples which you can run to check if
things are running correctly. To build them, you must enable the
`BUILD_CORE_SAMPLES` and/or `BUILD_ENGINE_SAMPLES` options.

### Testing

**CUBOS.** uses *doctest* for unit testing the engine. To build them, you must
enable the `BUILD_CORE_TESTS` and/or `BUILD_ENGINE_TESTS` options.

## Whats next?

We recommend you start by reading the @ref features "feature guide", which
introduces you to important concepts and features of the engine, such as what
is an ECS and how it is used in **CUBOS.**.

The @ref examples "examples page" is also a good place to go if you want to
see how specific parts of the engine are used in practice.
