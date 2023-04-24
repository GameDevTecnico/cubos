#include <cubos/core/io/glfw_window.hpp>
#include <cubos/core/io/window.hpp>

using namespace cubos::core::io;

Window cubos::core::io::openWindow(const std::string& title, const glm::ivec2& size)
{
    return std::make_shared<GLFWWindow>(title, size);
}

BaseWindow::BaseWindow()
{
    this->polled = false;
}

void BaseWindow::pushEvent(WindowEvent&& event)
{
    this->events.push_back(event);
}

std::optional<WindowEvent> BaseWindow::pollEvent()
{
    // Only poll events after pollEvent() has returned std::nullopt (or if it has never been called).
    if (!this->polled)
    {
        this->pollEvents();
        this->polled = true;
    }

    // No more events, we're done for now, but we need to poll again next time.
    if (this->events.empty())
    {
        this->polled = false;
        return std::nullopt;
    }

    WindowEvent event = std::move(this->events.front());
    this->events.pop_front();
    return event;
}
