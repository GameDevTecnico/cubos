# Introduction and Custom Bridges {#examples-engine-assets-bridge}

@brief Basic @ref assets-plugin plugin features and creating your own bridges.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/assets/bridge).

This example demonstrates how we can create a custom asset bridge to load
assets of a given type. More specifically, we'll go through how we can create a
bridge which loads `std::string`s from text files.

We define our bridge class by inheriting from @ref cubos::engine::FileBridge,
which offers a simple interface for loading and saving assets from files.

@snippet assets/bridge/main.cpp TextBridge

We pass `typeid(std::string)` to the base class, so that the engine knows which
type of assets this bridge can load. Then, we'll need to implement both the @ref
cubos::engine::FileBridge::loadFromFile "loadFromFile" and @ref
cubos::engine::FileBridge::saveToFile "saveToFile" methods.

In the first method, we receive the @ref cubos::engine::Assets
"assets manager" where we should store the loaded data, the handle to the asset
we're loading, and a stream to read the file data from.

@snippet assets/bridge/main.cpp TextBridge::loadFromFile

In the second method, we receive the @ref cubos::engine::Assets
"assets manager", the handle to the asset we're saving, and a stream to write
the file data to.

@snippet assets/bridge/main.cpp TextBridge::saveToFile

Now that we have our bridge type, we must register it with the assets manager
before using it.

@snippet assets/bridge/main.cpp Registering the bridge

After this system runs, any time we load an asset whose path ends with `.txt`,
the assets manager will use our bridge to load it.

In this sample we have a file `sample.txt` on the `assets/` directory containing the following text:

@include assets/bridge/assets/sample.txt

We also have a file `sample.txt.meta`, which describes the asset for the
engine. In this case, we only need to specify its UUID, which was generated on
a [UUID generator website](https://www.uuidgenerator.net/):

@include assets/bridge/assets/sample.txt.meta

Then, we can load it from our code:

@snippet assets/bridge/main.cpp Loading the asset

Some care must be taken when registering bridges or loading assets during the
startup phase. Systems which add bridges should be tagged with
`cubos.assets.bridge` so that they run before any assets are loaded.
Similarly, startup systems which load assets should be tagged with
`cubos.assets` so that they run after all bridges have been registered.
