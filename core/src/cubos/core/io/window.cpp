#include <cubos/core/io/window.hpp>
#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>

#include "glfw_window.hpp"

using namespace cubos::core::io;
using cubos::core::data::old::Deserializer;
using cubos::core::data::old::Serializer;

Window cubos::core::io::openWindow(const std::string& title, const glm::ivec2& size)
{
    return std::make_shared<GLFWWindow>(title, size);
}

std::string cubos::core::io::mouseButtonToString(MouseButton button)
{
#define MAP_BUTTON_TO_STRING(button, string)                                                                           \
    case MouseButton::button:                                                                                        \
        return string;
    switch (button)
    {
        MAP_BUTTON_TO_STRING(Left, "Left")
        MAP_BUTTON_TO_STRING(Right, "Right")
        MAP_BUTTON_TO_STRING(Middle, "Middle")
        MAP_BUTTON_TO_STRING(Extra1, "Extra1")
        MAP_BUTTON_TO_STRING(Extra2, "Extra2")
    default:
        return "Invalid";
    }
#undef MAP_KEY_TO_STRING
}

template <>
void cubos::core::data::old::serialize<MouseButton>(Serializer& ser, const MouseButton& obj, const char* name)
{
    ser.write(io::mouseButtonToString(obj), name);
}

MouseButton cubos::core::io::stringToMouseButton(const std::string& str)
{
#define MAP_STRING_TO_BUTTON(string, button)                                                                           \
    if (str == (string))                                                                                               \
        return MouseButton::button;
    MAP_STRING_TO_BUTTON("Left", Left)
    MAP_STRING_TO_BUTTON("Right", Right)
    MAP_STRING_TO_BUTTON("Middle", Middle)
    MAP_STRING_TO_BUTTON("Extra1", Extra1)
    MAP_STRING_TO_BUTTON("Extra2", Extra2)
    // else
    return MouseButton::Invalid;
#undef MAP_STRING_TO_BUTTON
}

template <>
void cubos::core::data::old::deserialize<MouseButton>(Deserializer& des, MouseButton& obj)
{
    std::string axis;
    des.read(axis);
    obj = io::stringToMouseButton(axis);
}

BaseWindow::BaseWindow()
{
    mPolled = false;
}

void BaseWindow::pushEvent(WindowEvent&& event)
{
    mEvents.push_back(event);
}

std::optional<WindowEvent> BaseWindow::pollEvent()
{
    // Only poll events after pollEvent() has returned std::nullopt (or if it has never been called).
    if (!mPolled)
    {
        this->pollEvents();
        mPolled = true;
    }

    // No more events, we're done for now, but we need to poll again next time.
    if (mEvents.empty())
    {
        mPolled = false;
        return std::nullopt;
    }

    WindowEvent event = mEvents.front();
    mEvents.pop_front();
    return event;
}
