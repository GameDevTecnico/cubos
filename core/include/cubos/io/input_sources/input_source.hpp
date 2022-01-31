#ifndef CUBOS_IO_INPUT_SOURCE_HPP
#define CUBOS_IO_INPUT_SOURCE_HPP

#include <cubos/event.hpp>
#include <cubos/io/window.hpp>
#include <cubos/io/keyboard.hpp>
#include <map>
#include <string>
#include <memory>
#include <variant>
#include <list>
#include <glm/glm.hpp>

namespace cubos::io
{
    class InputContext;
    class InputSource
    {
    public:
        virtual void subscribeEvents(cubos::io::Window* window) = 0;
        virtual void unsubscribeEvents(cubos::io::Window* window) = 0;
        virtual bool isTriggered() = 0;
        virtual InputContext createInputContext() = 0;
    };
} // namespace cubos::io
#endif // CUBOS_IO_INPUT_SOURCE_HPP