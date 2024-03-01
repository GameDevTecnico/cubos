/// @file
/// @brief Class @ref cubos::engine::SceneBridge.
/// @ingroup scene-plugin

#pragma once

#include <cubos/core/reflection/type_registry.hpp>

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
    class CUBOS_ENGINE_API SceneBridge : public FileBridge
    {
    public:
        /// @brief Constructs a bridge.
        /// @param components Component type registry.
        /// @param relations Relation type registry.
        SceneBridge(core::reflection::TypeRegistry components, core::reflection::TypeRegistry relations);

        /// @brief Returns the type registry used to deserialize components.
        /// @return Component type registry.
        core::reflection::TypeRegistry& components();

        /// @brief Returns the type registry used to deserialize relations.
        /// @return Relations type registry.
        core::reflection::TypeRegistry& relations();

    protected:
        bool loadFromFile(Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;
        bool saveToFile(const Assets& assets, const AnyAsset& handle, core::memory::Stream& stream) override;

    private:
        core::reflection::TypeRegistry mComponents;
        core::reflection::TypeRegistry mRelations;
    };
} // namespace cubos::engine
