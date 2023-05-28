#pragma once

#include <cubos/engine/input/bindings.hpp>

namespace cubos::engine
{
    class Input final
    {
    public:
        Input() = default;
        ~Input() = default;

        /// Clears all bindings
        void clear();

        /// Clears all bindings for a specific player
        /// @param player The player whose bindings will be cleared
        void clear(int player);

        /// Sets the bindings for a specific player
        /// @param player The player whose bindings will be set
        /// @param bindings The bindings to set
        void setBindings(int player, const InputBindings& bindings);

        /// Gets an action state for a specific player
        /// @param player The player whose action state will be retrieved
        /// @param actionName The name of the action
        /// @return True if the action exists and is pressed, false otherwise.
        bool isPressed(int player, const std::string& actionName) const;

        /// Gets an axis value for a specific player
        /// @param player The player whose axis value will be retrieved
        /// @param axisName The name of the axis
        /// @return The axis value if the axis exists, 0.0 otherwise.
        float axis(int player, const std::string& axisName) const;

        /// Gets the bindings
        /// @return The bindings
        const std::unordered_map<int, InputBindings>& getBindings() const;

    private:
        std::unordered_map<int, InputBindings> mBindings;
    };
} // namespace cubos::engine