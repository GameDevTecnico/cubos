/// @file
/// @brief Class @ref cubos::engine::ScenePath.
/// @ingroup scene-plugin

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Identifies an entity in a scene from a given node.
    ///
    /// A path is composed of zero or more segments, separated by '#', each segment being a node name.
    /// For example, an empty path refers to the root node, while "camera" refers to the root node's child named
    /// "camera", and "player#gun" refers to the child named "gun" of the child named "player" of the root node.
    ///
    /// @ingroup scene-plugin
    class CUBOS_ENGINE_API ScenePath
    {
    public:
        CUBOS_REFLECT;

        /// @brief Empty constructor.
        ScenePath() = default;

        /// @brief Copy constructor.
        ScenePath(const ScenePath&) = default;

        /// @brief Move constructor.
        ScenePath(ScenePath&&) = default;

        /// @brief Copy assignment.
        ScenePath& operator=(const ScenePath&) = default;

        /// @brief Move assignment.
        ScenePath& operator=(ScenePath&&) = default;

        /// @brief Constructs a path from a raw string.
        /// @param raw Raw string representing the path.
        static ScenePath fromRaw(const std::string& raw);

        /// @brief Appends a segment to the path.
        void append(const std::string& segment);

    private:
        std::string mRaw;
    };
} // namespace cubos::engine
