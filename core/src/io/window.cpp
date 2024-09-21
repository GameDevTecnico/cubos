#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>

#include "glfw_window.hpp"

using namespace cubos::core::io;

using cubos::core::io::MouseAxis;
using cubos::core::io::MouseButton;
using cubos::core::io::MouseState;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::Type;

Window cubos::core::io::openWindow(const std::string& title, const glm::ivec2& size, bool vSync)
{
    return std::make_shared<GLFWWindow>(title, size, vSync);
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

CUBOS_REFLECT_EXTERNAL_IMPL(MouseButton)
{
    return Type::create("cubos::core::io::MouseButton")
        .with(EnumTrait{}
                  .withVariant<MouseButton::Invalid>("Invalid")

                  .withVariant<MouseButton::Left>("Left")
                  .withVariant<MouseButton::Right>("Right")
                  .withVariant<MouseButton::Middle>("Middle")
                  .withVariant<MouseButton::Extra1>("Extra1")
                  .withVariant<MouseButton::Extra2>("Extra2")

                  .withVariant<MouseButton::Count>("Count"));
}

CUBOS_REFLECT_EXTERNAL_IMPL(MouseAxis)
{
    return Type::create("cubos::core::io::MouseAxis")
        .with(EnumTrait{}.withVariant<MouseAxis::X>("X").withVariant<MouseAxis::Y>("Y").withVariant<MouseAxis::Scroll>(
            "Scroll"));
}

CUBOS_REFLECT_EXTERNAL_IMPL(MouseState)
{
    return Type::create("cubos::core::io::MouseState")
        .with(EnumTrait{}
                  .withVariant<MouseState::Default>("Default")
                  .withVariant<MouseState::Locked>("Locked")
                  .withVariant<MouseState::Hidden>("Hidden"));
}

CUBOS_REFLECT_EXTERNAL_IMPL(WindowEvent)
{
    return Type::create("cubos::core::io::WindowEvent");
}

CUBOS_REFLECT_EXTERNAL_IMPL(Window)
{
    return Type::create("cubos::core::io::Window")
        .with(ConstructibleTrait::typed<Window>().withMoveConstructor().build());
}
