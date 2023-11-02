/// @file
/// @brief Class @ref cubos::engine::SceneBridge.
/// @ingroup scene-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/bridges/file.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace cubos::engine
{
    /// @brief Bridge which loads and saves @ref Scene assets.
    ///
    /// Scenes are stored as JSON files with two top-level objects:
    /// - "imports" - a dictionary of names to asset IDs of other scenes to import.
    /// - "entities" - a dictionary of names to serialized entities.
    ///
    /// Entities imported from sub-scenes can be overriden by defining them in the current scene.
    /// For example:
    ///
    /// @code{.json}
    /// {
    ///     "imports": {
    ///         "foo": "6f42ae5a-59d1-5df3-8720-83b8df6dd536"
    ///     },
    ///     "entities": {
    ///         "baz": {
    ///             "cubos::engine::Position": {
    ///                 "x": 10
    ///             }
    ///         },
    ///         "foo.bar": {
    ///             "Parent": "baz",
    ///         }
    ///     }
    /// }
    /// @endcode
    ///
    /// This scene will import the sub-scene with ID `6f42ae5a-59d1-5df3-8720-83b8df6dd536`, and
    /// prefix all of its entities with `foo.`. The entity `foo.bar` will override the entity `bar`
    /// from the imported scene, while the entity `baz` will be added to the scene.
    ///
    /// @ingroup scene-plugin
    class SceneBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        ///
        SceneBridge()
            : FileBridge(core::reflection::reflect<Scene>())
        {
        }

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
    };
} // namespace cubos::engine
