# Loading Reflectable Assets {#examples-engine-assets-json}

@brief Loading reflectable assets from JSON.

@see Full source code [here](https://github.com/GameDevTecnico/cubos/tree/main/engine/samples/assets/json).

We'll use the following type as an example:

@snippet assets/json/main.cpp Asset type

Then, we must register a bridge for this type. We provide @ref
cubos::engine::JSONBridge "JSONBridge" for easily loading and saving
reflectable assets as JSON.

@snippet assets/json/main.cpp Register bridge

With the bridge registered, we can just load it from its handle:

@snippet assets/json/main.cpp Loading the asset

These sytems are configured the usual way, as explained in @ref
examples-engine-assets-bridge.
