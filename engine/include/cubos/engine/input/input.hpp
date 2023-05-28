#pragma once

#include <cubos/core/io/keyboard.hpp>

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
        /// @param bindings The bindings to set
        /// @param player The player whose bindings will be set
        void bind(const InputBindings& bindings, int player = 0);

        /// Gets an action state for a specific player
        /// @param actionName The name of the action
        /// @param player The player whose action state will be retrieved
        /// @return True if the action exists and is pressed, false otherwise.
        bool pressed(const std::string& actionName, int player = 0) const;

        /// Gets an axis value for a specific player
        /// @param axisName The name of the axis
        /// @param player The player whose axis value will be retrieved
        /// @return The axis value if the axis exists, 0.0 otherwise.
        float axis(const std::string& axisName, int player = 0) const;

        /// Gets the bindings
        /// @return The bindings
        const std::unordered_map<int, InputBindings>& bindings() const;

    private:
        struct BindingIndex
        {
            std::string name;
            int player;
            bool negative = false;
        };

        void removeBoundPlayer(std::vector<BindingIndex>& boundKeys, int player);

        std::unordered_map<int, InputBindings> mBindings;

        // core::io::Modifiers mModifiers = core::io::Modifiers::None;
        std::unordered_map<core::io::Key, std::vector<BindingIndex>> mBoundActions;
        std::unordered_map<core::io::Key, std::vector<BindingIndex>> mBoundAxes;
    };
} // namespace cubos::engine