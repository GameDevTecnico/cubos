/// @file
/// @brief Class @ref cubos::engine::InputAxis.
/// @ingroup input-plugin

#pragma once

#include <vector>

#include <cubos/core/io/gamepad.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/input/action.hpp>

namespace cubos::engine
{
    /// @brief Stores the state of a single input axis, such as "move forward" or "move right".
    ///
    /// Can be bound to multiple keys, and will have a value in the range [-1, 1] based on the the
    /// state of its bindings.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputAxis final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Updates the state of the axis.
        /// @param window Window to get the value from.
        void update(const core::io::Window& window);

        /// @brief Gets the value.
        /// @return Value.
        float value() const;

        std::vector<InputAxisSource> axes;
        InputAction positive;
        InputAction negative;

    private:
        float mValue{0.0F};
    };

    static const InputAxis NullAxis = InputAxis{};
} // namespace cubos::engine
