# Assets {#engine-plugins-assets}

The \ref cubos::engine::plugins::assets "assets" plugin adds the \ref
cubos::engine::data::AssetManager "AssetManager" resource, which is manages
loading and unloading of assets.

It also adds a startup system to the stage `"import-meta"`. This system calls
the \ref cubos::engine::data::AssetManager::importMeta
"AssetManager::importMeta" function on the virtual file specified by the
setting `assets.path` (default: `"/assets"`).

## Meta files

Meta files are files which describe and define what assets are available.
You can have multiple meta files on your project. Meta files are expected to be
in the JSON format.

An example meta file:
```json
{
    {
        "palette": {
            "type": "Palette",
            "usage": "Static",
            "params": {
                "path": "/assets/main.pal"
            }
        },
        "car": {
            "type": "Grid",
            "usage": "Static",
            "params": {
                "path": "/assets/car.grd"
            }
        }
    }
}
```

Each entry has its identifier, and specifies the type of the asset, its usage
mode and the parameters to be passed to the asset loader.

## Loading assets

Assets are loaded using the \ref cubos::engine::data::AssetManager::load
"AssetManager::load" function. This function takes the identifier of the asset
and returns a \ref cubos::engine::data::Asset "Asset" object. This object as
a handle to the asset, which guarantees that the asset will not be unloaded
while the handle is alive.

### Usage modes

Assets can be in one of two usage modes: `Static` or `Dynamic`.
`Static` assets are never unloaded, while `Dynamic` assets are unloaded when
they are not used for a while.

## Storing assets

Let's say you have generated a grid of voxels procedurally and you want to get
an asset handle to it. You can use the \ref
cubos::engine::data::AssetManager::store "AssetManager::store" method to store
the data in the asset manager, which then returns an \ref
cubos::engine::data::Asset "Asset" pointing to the data.

## Defining asset types

If you want to define your own asset types, you can do so by adding a
`static constexpr const char* TypeName` member to your type, with the name you
want to use to identify the asset type. You also need to create a child class
of the \ref cubos::engine::data::Loader "Loader" class, which will be used to
load your asset. Finally, add `using Loader = MyLoader;` to your asset type to
tell the asset manager which loader to use.

For example, the \ref cubos::engine::data::Grid "Grid" asset type is defined
like this:

```cpp
struct Grid
{
    static constexpr const char* TypeName = "Grid";
    using Loader = impl::GridLoader;

    core::gl::Grid grid;           ///< Raw grid data.
    gl::RendererGrid rendererGrid; ///< Handle of the grid uploaded to the GPU.
};
```


