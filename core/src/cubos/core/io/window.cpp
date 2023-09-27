#include <cubos/core/io/window.hpp>

#include "glfw_window.hpp"

using namespace cubos::core::io;

Window cubos::core::io::openWindow(const std::string& title, const glm::ivec2& size)
{
    return std::make_shared<GLFWWindow>(title, size);
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
