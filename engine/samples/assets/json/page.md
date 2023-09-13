# Loading Serializable Assets {#examples-engine-assets-json}

@brief Loading serializable assets from JSON.

We'll use the following type as an example:

@snippet assets/json/main.cpp Asset type

We can make it serializable by implementing the following specializations:

@snippet assets/json/main.cpp Serialization definition

Then, we must register a bridge for this type. We provide @ref
cubos::engine::JSONBridge "JSONBridge" for easily loading and saving
serializable assets as JSON.

@snippet assets/json/main.cpp Register bridge

With the bridge registered, we can just load it from its handle:

@snippet assets/json/main.cpp Loading the asset

These sytems are configured the usual way, as explained in @ref
examples-engine-assets-bridge.

@snippet assets/json/main.cpp Configuration
