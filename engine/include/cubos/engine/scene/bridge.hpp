#pragma once

#include <cubos/engine/assets/bridge.hpp>

namespace cubos::engine
{
    /// A bridge to load and save `Scene` assets.
    ///
    /// @details Scenes are stored as JSON files with two top-level objects:
    /// - "imports" - a dictionary of names to asset IDs of other scenes to import.
    /// - "entities" - a dictionary of names to serialized entities.
    ///
    /// Entities imported from sub-scenes can be overriden by defining them in the
    /// current scene. For example:
    ///
    ///     {
    ///         "imports": {
    ///             "foo": "6f42ae5a-59d1-5df3-8720-83b8df6dd536"
    ///         },
    ///         "entities": {
    ///             "baz": {
    ///                 "cubos/position": {
    ///                     "x": 1,
    ///                     "y": 2,
    ///                     "z": 3
    ///                 }
    ///             },
    ///             "foo.bar": {
    ///                 "cubos/parent": "baz",
    ///             }
    ///         }
    ///     }
    ///
    /// This scene will import the scene with ID "6f42ae5a-59d1-5df3-8720-83b8df6dd536", and prefix
    /// all of its entities with "foo.". The entity "foo.bar" will override the entity "bar" from
    /// the imported scene, while the entity "baz" will be added to the scene.
    class SceneBridge : public AssetBridge
    {
    public:
        bool load(Assets& assets, const AnyAsset& handle) override;
        bool save(const Assets& assets, const AnyAsset& handle) override;
    };
} // namespace cubos::engine
