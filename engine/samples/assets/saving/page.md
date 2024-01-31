# Creating and Saving {#examples-engine-assets-saving}

@brief Creating and saving assets.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/assets/saving).

This example demonstrates how a new asset can be created programatically and how it
can be saved to the assets directory, which is useful while working on tools
such as **TESSERATOS**

Before we go any further, if we want to save assets to the filesystem, we must
allow assets to be modified. This is done through the following setting:

@snippet assets/saving/main.cpp Setting

We'll use the following asset type as an example, with a @ref
cubos::engine::JSONBridge "JSONBridge" registered for it with the extension
`.int`.

@snippet assets/saving/main.cpp Asset type

First, we'll create an asset of this type:

@snippet assets/saving/main.cpp Create a new asset

Then, we'll assign it a path and save it. It's important that the path ends with
the correct extension, so that @ref cubos::engine::Assets "Assets" knows which
bridge to use when loading it.

@snippet assets/saving/main.cpp Save the asset

With this, the files `sample/sample.int` and `sample/sample.int.meta` should
have appeared on the `assets/` directory. The `.meta` file contains the UUID of
the asset, which is used by the engine to identify it.

Try running the sample yourself to see the files being created!
