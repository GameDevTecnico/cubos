#ifndef CUBOS_IO_DOUBLE_AXIS_HPP
#define CUBOS_IO_DOUBLE_AXIS_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>
#include <cubos/io/input_sources/input_source.hpp>

namespace cubos::io
{
    class DoubleAxis : public InputSource
    {
    public:
        DoubleAxis(cubos::io::MouseAxis horizontalAxis, cubos::io::MouseAxis verticalAxis);

        bool isTriggered() override;
        void subscribeEvents(cubos::io::Window* window) override;
        void unsubscribeEvents(cubos::io::Window* window) override;
        InputContext createInputContext() override;

    private:
        bool wasTriggered = false;
        float xPos;
        float yPos;
        using Axis = std::variant<cubos::io::MouseAxis>;
        Axis horizontalAxis;
        Axis verticalAxis;
        void handleHorizontalAxis(float xPos);
        void handleVerticalAxis(float yPos);
    };
} // namespace cubos::io

#endif // CUBOS_IO_DOUBLE_AXIS_HPP