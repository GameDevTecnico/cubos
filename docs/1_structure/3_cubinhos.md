# Cubinhos {#cubinhos}

*Cubinhos* is the CLI tool used to work with *CUBOS.* At the moment, it contains
the following commands:
- `cubinhos help`: shows the help message;
- `cubinhos convert`: converts a `.qb` voxel file into the internal format used
  by CUBOS., `.grd` and `.pal`.
- `cubinhos embed`: utility used to embed files directly into an executable for
  use with the `EmbeddedArchive`.
- `cubinhos generate`: generates component definition boilerplate code.

## Convert

The need for this tool arose from the fact that **CUBOS.** works with a single
palette of voxel materials, and voxel objects refer to specific materials by
their index in this palette. For example, if we had a single voxel red object,
the red color wouldn't be specified in the voxel object. Instead, this object
would specify that its material is `1`, and the palette would specify that the
material `1` is red.

The issue is, voxel formats like `.qb` and `.vox` are not designed to store
only the indices of materials, but the whole material definition, per voxel.
This conflicts with the way **CUBOS.** works, so we need to convert these
formats into the internal format used by **CUBOS.**. This is where
`cubinhos convert` comes in. It takes a `.qb` voxel file and splits it into a
palette (`.pal`), and one or more voxel grids (`.grd`).

### Usage

This guide will be based on four use cases:
1. loading a `.qb` file and creating a new `.pal`;
2. loading a `.qb` file and updating an existing `.pal`;
3. loading a `.qb` file and using, without editing, an existing `.pal`;
4. querying the contents of `.qb` model;

#### Example 1: Creating a new palette

This use case is most common when importing the first `.qb` file for a new
project. There is no palette file yet, so we need to create one. Lets say the
model we want to convert is a `car.qb`, which contains a single voxel object.

```bash
$ cubinhos convert car.qb -p main.pal -g car.grd -w
```

First, we specify the `.qb` file to convert. Then, we specify the name of the
palette we will be outputting. We also need to specify the output file for the
voxel grid. Finally, we pass the `-w` flag which allows us to write to the
palette file (which is necessary since there is none at the moment).

#### Example 2: Updating an existing palette

Lets say we now want to import a `street.qb` model which contains two voxel
grids: the first one is a road and the second one is a tree. Since we already
have a palette, we want to add any new materials to it.

```bash
$ cubinhos convert street.qb -p main.pal -g0 road.grd -g1 tree.grd -w
```

Once again, we first specify the `.qb` file to convert. Then, we specify the
name of the palette we will be using, which is the same as the one we created
in the previous example. This time, we have two grids in the `.qb` file, so we
need to specify the output files for both. This is done by adding an index in
front of `-g` (e.g. `-g0` for the first grid, `-g1` for the second). Finally,
we once again pass the `-w` flag which allows the palette file to be updated.

#### Example 3: Using an existing palette

What if we want to add another car model, but without adding materials to the
palette? To do this, we will need to ommit the `-w` flag. This way, the palette
will be read, and the most similar materials found will be used. By default,
this will only succeed if the materials match exactly.

```bash
$ cubinhos convert car2.qb -p main.pal -g car2.grd
```

If this fails, you may see the error message:

```
Failed to convert grid 0 from its palette to the palette chosen.
```

One work around is lowering the threshold for material matching. If you want to
allow for slightly different materials to be considered as the same, you can
pass the `-s <SIMILARITY>` option. This value is a number from `0` to `1`,
where `0` is completely different and `1` is completely the same. It represents
the minimum similarity between two materials to consider them the same (default
is `1`).

So, if the materials which aren't matching are very similar, you could add, for
example, `-s 0.9` to the previous command. This way, the material chosen from
the palette will have a similarity of at least `0.9` with the material in the
original model.

#### Example 4: Querying the contents of a model

You may want to check the contents of a `.qb` file before converting it. One
easy way to do this is to use the `-v` (verbose) flag. This can be added to any
of the previous commands to get extra information about what is going on.

It can also be used without any other options, in which case the program will
just print the contents of the `.qb` file. For example:

```bash
$ cubinhos convert car.qb -v
Found 1 QB matrices.
Matrix 0:
- Position: -7 -5 -16
- Grid size: 15x11x32
- Palette size: 10
```

This tells us that the `car.qb` model contains exactly one voxel grid, that its
position within the model is `(-7, -5, -16)`, that its size is `15x11x32`, and
that it uses `10` different materials.

## Embed

The `cubinhos embed` tool is used to embed files directly into an executable for
use with the `EmbeddedArchive`. This is useful for example when you want to ship
a game with a set of assets, but don't want to have to distribute them as
separate files. This way, you are able to ship a single executable file.

### Usage

This tool takes a file and generates a C++ source file which registers a new
`EmbeddedArchive`, outputting it to the standard output. This source file can
then be compiled and linked with your executable.

If no name for the archive is specified, the name of the file will be used. For
example, if you run `cubinhos embed logo.png > logo.cpp`, the name of the
archive will be `logo.png`.

The resulting archive can then be mounted like this:

```cpp
#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/embedded_archive.hpp>

int main()
{
    using namespace cubos::core::data;

    FileSystem::mount("/logo.png", std::make_shared<EmbeddedArchive>("logo.png"));

    // ...

    return 0;
}
```

It's also possible to embed a whole directory, in which case you will need to
use the `-r` flag. This will recursively embed all files in the directory.

Checkout the `embedded_archive` sample for a complete example.

## Generate

For a type to be usable as a component, it must satisfy the following
requirements:
- it must be (de)serializable, taking optionally some context necessary (such
  as the mapping between `Entity`s and their IDs, or a function to read or
  write handles).
- it must define a storage type, which will then be used to store the
  component's data in the `World`.
- it must call the `CUBOS_REGISTER_COMPONENT` macro, passing the ID of the
  component type, which associates the type with a unique ID.

This means that, before `cubinhos generate`, component definitions looked like:

```cpp
struct MyComponent
{
    int x;
    float y;
    std::string z;
};

// The serialization functions must be defined in this namespace.
namespace cubos::core::data
{
    inline static void serialize(Serializer& s, const MyComponent& c, const char* name)
    {
        s.beginObject(name);
        s.write(c.x);
        s.write(c.y);
        s.write(c.z);
        s.endObject();
    }

    inline static void deserialize(Deserializer& d, MyComponent& c)
    {
        d.beginObject();
        d.read(c.x);
        d.read(c.y);
        d.read(c.z);
        d.endObject();
    }
}

// The storage type must be defined in this namespace.
namespace cubos::core::ecs
{
    template <>
    struct ComponentStorage<MyComponent>
    {
        using Type = VecStorage<MyComponent>;
    };
}

CUBOS_REGISTER_COMPONENT(MyComponent, "my_component");
```

This amount of boilerplate is not acceptable, specially for user-facing code.
The `cubinhos generate` command makes use of a recent C++ feature called
[attributes](https://en.cppreference.com/w/cpp/language/attributes). Attributes
are usually used to provide hints to the compiler. However, on our case, we're
using them to mark types as components.

With `cubinhos generate`, the same component definition can be written as:

```cpp
// Must be included in the file where the component is defined.
#include <components/base.hpp>

struct [[cubos::component("my_component", VecStorage)]] MyComponent
{
    int x;
    float y;
    std::string z;
};
```

This tool then searches for all types marked with the `cubos::component` and
generates a header file for each one. In this case, a header file would be
generated which could be included with `#include <components/my_component.hpp>`.

When using the component, the user must include the generated header file, and
not the original one. Otherwise, the compiler won't see the generated code.

If the component is identified with a slash, as, for example,
`game/my_component`, the generated header file will be placed in the
`components/game` directory. This is useful for organizing components into
different modules. For example, `cubos-engine` names all its components with the
`cubos/` directory.

### CMake

This tool can be configured to run automatically when building the project. To
do this, you can include the `CubinhosGenerate` module in your `CMakeLists.txt`
and call the `cubinhos_generate` function with your target and the directory
where the base component headers are located.

```cmake
include(CubinhosGenerate)

cubinhos_generate(my_target ${CMAKE_CURRENT_SOURCE_DIR}/components)
```
