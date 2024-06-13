#include "window.hpp"

#include <imgui.h>

#include <cubos/core/log.hpp>

using namespace cubos::core::gl;
using namespace cubos::core::io;
using namespace tesseratos;

GameWindow::GameWindow(Window window)
    : mWindow{window}
    , mRenderDevice{window->renderDevice()}
    , mStartTime{window->time()}
{
}

void GameWindow::reset()
{
    mMouseState = MouseState::Default;
    mCursor = nullptr;
    this->setFocused(false);
}

void GameWindow::handleEvent(WindowEvent event)
{
    if (std::holds_alternative<ResizeEvent>(event))
    {
        // Do nothing, external resizes don't affect the window.
    }
    else if (auto* mouseMove = std::get_if<MouseMoveEvent>(&event))
    {
        this->pushEvent(MouseMoveEvent{mouseMove->position - static_cast<glm::ivec2>(mMinCoords)});
    }
    else
    {
        this->pushEvent(std::move(event));
    }
}

void GameWindow::setMinCoords(glm::uvec2 coords)
{
    mMinCoords = coords;
}

void GameWindow::setFocused(bool focused)
{
    mFocused = focused;

    if (mFocused)
    {
        mWindow->mouseState(mMouseState);
        mWindow->cursor(mCursor);
    }
    else
    {
        mWindow->mouseState(MouseState::Default);
        mWindow->cursor(nullptr);
    }
}

void GameWindow::resize(glm::uvec2 size)
{
    if (mSize != size)
    {
        mSize = size;
        mRenderDevice.createSwapChain(size);
        this->pushEvent(ResizeEvent{size});
    }
}

Texture2D GameWindow::output() const
{
    return mRenderDevice.output();
}

bool GameWindow::focused() const
{
    return mFocused;
}

void GameWindow::pollEvents()
{
    // Do nothing.
}

void GameWindow::swapBuffers()
{
    mRenderDevice.swapBuffers();
}

cubos::core::gl::RenderDevice& GameWindow::renderDevice() const
{
    return mRenderDevice;
}

glm::uvec2 GameWindow::size() const
{
    return mSize;
}

glm::uvec2 GameWindow::framebufferSize() const
{
    return mSize;
}

float GameWindow::contentScale() const
{
    return mWindow->contentScale();
}

bool GameWindow::shouldClose() const
{
    return false;
}

double GameWindow::time() const
{
    return mWindow->time() - mStartTime;
}

void GameWindow::mouseState(MouseState state)
{
    mMouseState = state;

    if (mFocused)
    {
        mWindow->mouseState(state);
    }
}

MouseState GameWindow::mouseState() const
{
    return mMouseState;
}

glm::ivec2 GameWindow::getMousePosition()
{
    return mWindow->getMousePosition() - static_cast<glm::ivec2>(mMinCoords);
}

void GameWindow::setMousePosition(glm::ivec2 pos)
{
    mWindow->setMousePosition(pos + static_cast<glm::ivec2>(mMinCoords));
}

std::shared_ptr<Cursor> GameWindow::createCursor(Cursor::Standard standard)
{
    return mWindow->createCursor(standard);
}

void GameWindow::cursor(std::shared_ptr<Cursor> cursor)
{
    mCursor = cursor;

    if (mFocused)
    {
        mWindow->cursor(cursor);
    }
}

void GameWindow::clipboard(const std::string& text)
{
    return mWindow->clipboard(text);
}

const char* GameWindow::clipboard() const
{
    return mWindow->clipboard();
}

Modifiers GameWindow::modifiers() const
{
    if (mFocused)
    {
        return mWindow->modifiers();
    }
    else
    {
        return Modifiers::None;
    }
}

bool GameWindow::pressed(Key key, Modifiers modifiers) const
{
    if (mFocused)
    {
        return mWindow->pressed(key, modifiers);
    }
    else
    {
        return false;
    }
}

bool GameWindow::gamepadState(int gamepad, GamepadState& state) const
{
    return mWindow->gamepadState(gamepad, state);
}
