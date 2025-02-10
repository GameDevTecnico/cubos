/// @file
/// @brief Class @ref cubos::engine::InputBindings.
/// @ingroup input-plugin

#pragma once

#include <unordered_map>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/input/action.hpp>
#include <cubos/engine/input/axis.hpp>
#include <cubos/engine/input/transparent_equal.hpp>
#include <cubos/engine/input/transparent_hash.hpp>

namespace cubos::engine
{
    /// @brief Stores the input bindings for a single player.
    ///
    /// It contains a map of input actions and a map of input axes.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputBindings final
    {
    public:
        CUBOS_REFLECT;

        InputBindings() = default;
        ~InputBindings() = default;

        /// @brief Gets the input actions map.
        /// @return Input actions map.
        const std::unordered_map<std::string, InputAction, InputTransparentHash, InputTransparentEqual>& actions() const;

        /// @brief Gets the input axes map.
        /// @return Input axes map.
        const std::unordered_map<std::string, InputAxis, InputTransparentHash, InputTransparentEqual>& axes() const;

        /// @brief Gets the input actions map.
        /// @return Input actions map.
        std::unordered_map<std::string, InputAction, InputTransparentHash, InputTransparentEqual>& actions();

        /// @brief Gets the input axes map.
        /// @return Input axes map.
        std::unordered_map<std::string, InputAxis, InputTransparentHash, InputTransparentEqual>& axes();

    private:
        std::unordered_map<std::string, InputAction, InputTransparentHash, InputTransparentEqual> mActions;
        std::unordered_map<std::string, InputAxis, InputTransparentHash, InputTransparentEqual> mAxes;
    };
} // namespace cubos::engine
