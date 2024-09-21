/// @file
/// @brief Class @ref cubos::engine::InputCombination.
/// @ingroup input-plugin

#pragma once

#include <vector>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/input/sources/press.hpp>

namespace cubos::engine
{
    /// @brief Stores set of pressable input sources of a single input combination.
    ///
    /// Composed of multiple keys and buttons, and will be considered "pressed" if all of them are pressed.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputCombination final
    {
    public:
        CUBOS_REFLECT;

        ~InputCombination() = default;

        /// @brief Constructs without any keys or buttons.
        InputCombination() = default;

        /// @brief Constructs from a vector of pressable input sources.
        /// @param sources Vector of pressable input sources.
        InputCombination(std::vector<InputPressSource> sources)
            : mSources(std::move(sources))
        {
        }

        /// @brief Gets the value of the combination.
        /// @param window Window to get the value from.
        /// @return Value of the combination.
        bool pressed(const core::io::Window& window) const;

    private:
        std::vector<InputPressSource> mSources;
    };

} // namespace cubos::engine