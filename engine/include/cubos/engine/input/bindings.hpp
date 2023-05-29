/// @file
#pragma once

#include <unordered_map>

#include <cubos/engine/input/action.hpp>
#include <cubos/engine/input/axis.hpp>

namespace cubos::engine
{

    class InputBindings final
    {
    public:
        InputBindings() = default;
        ~InputBindings() = default;

        const std::unordered_map<std::string, InputAction>& actions() const;
        const std::unordered_map<std::string, InputAxis>& axes() const;

        std::unordered_map<std::string, InputAction>& actions();
        std::unordered_map<std::string, InputAxis>& axes();

        std::string toString() const;

    private:
        std::unordered_map<std::string, InputAction> mActions;
        std::unordered_map<std::string, InputAxis> mAxes;
    };
} // namespace cubos::engine
