# Loading and Saving JSON Assets {#examples-engine-assets-json}

@brief Loading and saving serializable assets from and to JSON.

This example demonstrates how you can load and save assets of a serializable
type from and to JSON files.

Before we go any further, if we want to save assets to the filesystem, we must
allow assets to be modified. This is done through the following setting:

@snippet assets/json/main.cpp Setting

We'll use the following type as an example:

@snippet assets/json/main.cpp Asset type

We can make it serializable by implementing the following specializations:

@snippet assets/json/main.cpp Serialization definition

Then, we must register a bridge for this type. Since we want to load and save
the data as JSON, we'll use @ref cubos::engine::JSONBridge "JSONBridge".

@snippet assets/json/main.cpp Register bridge

First, we'll create an asset of this type:

@snippet assets/json/main.cpp Create a new asset

Then, we'll assign it a path and save it. Its important that the path ends with
the correct extension, so that @ref cubos::engine::Assets "Assets" knows which
bridge to use when loading it.

@snippet assets/json/main.cpp Save the asset

With this, the files `sample/asset.my` and `sample/asset.my.meta` should have
appeared on the `assets/` directory. The `.meta` file contains the UUID of the
asset, which is used by the engine to identify it.

To test whether the asset can be loaded correctly from the saved data, we'll
force it to be reloaded, using @ref cubos::engine::Assets::invalidate
"Assets::invalidate".

@snippet assets/json/main.cpp Force reload

Now, when we try reading the asset data again, it will be loaded from the
previously saved file.

@snippet assets/json/main.cpp Read the asset

Finally, the engine is configured the following way:

@snippet assets/json/main.cpp Run

Try running the sample yourself to see the loaded data!

