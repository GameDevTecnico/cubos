#ifndef CUBOS_INPUT_BUTTON_PRESS_HPP
#define CUBOS_INPUT_BUTTON_PRESS_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/input/input_sources/input_source.hpp>

namespace cubos::input
{
    class ButtonPress : public InputSource
    {
    public:
        std::variant<cubos::io::Key, cubos::io::MouseButton> button;

        ButtonPress(cubos::io::Key key);
        ButtonPress(cubos::io::MouseButton button);

        bool isTriggered() override;
        void subscribeEvents(cubos::io::Window* window) override;
        void unsubscribeEvents(cubos::io::Window* window) override;
        InputContext createInputContext() override;

    private:
        bool wasTriggered = false;
        void handleButtonDown();
    };
} // namespace cubos::input
#endif // CUBOS_INPUT_BUTTON_PRESS_HPP