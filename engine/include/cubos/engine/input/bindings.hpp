/// @file
/// @brief Class @ref cubos::engine::InputBindings.
/// @ingroup input-plugin

#pragma once

#include <unordered_map>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/data/transparent/string_hash.hpp>
#include <cubos/core/data/transparent/string_equal.hpp>

#include <cubos/engine/input/action.hpp>
#include <cubos/engine/input/axis.hpp>


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
        const std::unordered_map<std::string, InputAction, core::data::StringTransparentHash, core::data::StringTransparentEqual>& actions() const;

        /// @brief Gets the input axes map.
        /// @return Input axes map.
        const std::unordered_map<std::string, InputAxis, core::data::StringTransparentHash, core::data::StringTransparentEqual>& axes() const;

        /// @brief Gets the input actions map.
        /// @return Input actions map.
        std::unordered_map<std::string, InputAction, core::data::StringTransparentHash, core::data::StringTransparentEqual>& actions();

        /// @brief Gets the input axes map.
        /// @return Input axes map.
        std::unordered_map<std::string, InputAxis, core::data::StringTransparentHash, core::data::StringTransparentEqual>& axes();

    private:
        std::unordered_map<std::string, InputAction, core::data::StringTransparentHash, core::data::StringTransparentEqual> mActions;
        std::unordered_map<std::string, InputAxis, core::data::StringTransparentHash, core::data::StringTransparentEqual> mAxes;
    };
} // namespace cubos::engine
