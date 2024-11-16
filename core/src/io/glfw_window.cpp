#include "glfw_window.hpp"

#include <glad/glad.h>

#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/tel/logging.hpp>

#include "../gl/ogl_render_device.hpp"

using namespace cubos::core;
using namespace cubos::core::io;

#ifdef CUBOS_CORE_GLFW

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void charCallback(GLFWwindow* window, unsigned int codepoint);
static void joystickCallback(int id, int event);
static void updateMods(GLFWWindow* handler, int glfwMods);
static MouseButton glfwToCubosMouseButton(int button);
static int cubosToGlfwMouseButton(MouseButton button);
static Key glfwToCubosKey(int key);
static int cubosToGlfwKey(Key key);

static GLFWWindow* currentWindow = nullptr;

#endif // CUBOS_CORE_GLFW

#define UNSUPPORTED()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Unsupported when building without GLFW");                                                      \
        abort();                                                                                                       \
    } while (0)

static void errorCallback(int errorCode, const char* description)
{
    CUBOS_ERROR("GLFW Error {}: {}", errorCode, description);
}

GLFWWindow::GLFWWindow(const std::string& title, const glm::ivec2& size, bool vSync)
{
#ifdef CUBOS_CORE_GLFW
    CUBOS_ASSERT(currentWindow == nullptr, "Only one window is supported at the moment");

    glfwSetErrorCallback(errorCallback);

    if (glfwInit() == 0)
    {
        CUBOS_CRITICAL("glfwInit() failed");
        abort();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

    // TODO: handle mode (fullscreen, ...)
    mHandle = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
    if (mHandle == nullptr)
    {
        CUBOS_CRITICAL("glfwCreateWindow() failed");
        abort();
    }

    // Create OpenGL render device
    glfwMakeContextCurrent(mHandle);
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        CUBOS_CRITICAL("OpenGL loader failed");
        abort();
    }
    mRenderDevice = new gl::OGLRenderDevice();

    if (!vSync)
    {
        glfwSwapInterval(0);
    }

    // Set input callbacks
    glfwSetWindowUserPointer(mHandle, (void*)this);
    glfwSetKeyCallback(mHandle, keyCallback);
    glfwSetCursorPosCallback(mHandle, mousePositionCallback);
    glfwSetMouseButtonCallback(mHandle, mouseButtonCallback);
    glfwSetScrollCallback(mHandle, scrollCallback);
    glfwSetFramebufferSizeCallback(mHandle, framebufferSizeCallback);
    glfwSetCharCallback(mHandle, charCallback);
    glfwSetJoystickCallback(joystickCallback);
    currentWindow = this;

    for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i)
    {
        if (glfwJoystickIsGamepad(i) != GLFW_FALSE)
        {
            this->pushEvent(GamepadConnectionEvent{.gamepad = i, .connected = true});
        }
    }
#else
    UNSUPPORTED();
#endif
}

GLFWWindow::~GLFWWindow()
{
#ifdef CUBOS_CORE_GLFW
    delete mRenderDevice;
    glfwTerminate();
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::pollEvents()
{
#ifdef CUBOS_CORE_GLFW
    glfwPollEvents();
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::swapBuffers()
{
#ifdef CUBOS_CORE_GLFW
    glfwSwapBuffers(mHandle);
#else
    UNSUPPORTED();
#endif
}

gl::RenderDevice& GLFWWindow::renderDevice() const
{
#ifdef CUBOS_CORE_GLFW
    return *mRenderDevice;
#else
    UNSUPPORTED();
#endif
}

glm::uvec2 GLFWWindow::size() const
{
#ifdef CUBOS_CORE_GLFW
    int width;
    int height;
    glfwGetWindowSize(mHandle, &width, &height);
    return {width, height};
#else
    UNSUPPORTED();
#endif
}

glm::uvec2 GLFWWindow::framebufferSize() const
{
#ifdef CUBOS_CORE_GLFW
    int width;
    int height;
    glfwGetFramebufferSize(mHandle, &width, &height);
    return {width, height};
#else
    UNSUPPORTED();
#endif
}

float GLFWWindow::contentScale() const
{
#ifdef CUBOS_CORE_GLFW
    float xscale;
    float yscale;
    glfwGetWindowContentScale(mHandle, &xscale, &yscale);
    // xscale and yscale should be the same, so only return one
    return xscale;
#else
    UNSUPPORTED();
#endif
}

bool GLFWWindow::shouldClose() const
{
#ifdef CUBOS_CORE_GLFW
    return glfwWindowShouldClose(mHandle) != 0;
#else
    UNSUPPORTED();
#endif
}

double GLFWWindow::time() const
{
#ifdef CUBOS_CORE_GLFW
    return glfwGetTime();
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::mouseState(MouseState state)
{
#ifdef CUBOS_CORE_GLFW
    int cursorState;
    switch (state)
    {
    case MouseState::Default:
        cursorState = GLFW_CURSOR_NORMAL;
        break;
    case MouseState::Locked:
        cursorState = GLFW_CURSOR_DISABLED;
        break;
    case MouseState::Hidden:
        cursorState = GLFW_CURSOR_HIDDEN;
        break;
    default:
        CUBOS_ERROR("Unknown MouseState, doing nothing");
        return;
    }
    glfwSetInputMode(mHandle, GLFW_CURSOR, cursorState);
#else
    UNSUPPORTED();
#endif
}

MouseState GLFWWindow::mouseState() const
{
#ifdef CUBOS_CORE_GLFW
    switch (glfwGetInputMode(mHandle, GLFW_CURSOR))
    {
    case GLFW_CURSOR_NORMAL:
        return MouseState::Default;
    case GLFW_CURSOR_DISABLED:
        return MouseState::Locked;
    case GLFW_CURSOR_HIDDEN:
        return MouseState::Hidden;
    default:
        CUBOS_WARN("Unknown GLFW cursor state, returning MouseState::Default");
        return MouseState::Default;
    }
#else
    UNSUPPORTED();
#endif
}

glm::ivec2 GLFWWindow::getMousePosition()
{
#ifdef CUBOS_CORE_GLFW
    double xpos = 0;
    double ypos = 0;
    glfwGetCursorPos(mHandle, &xpos, &ypos);
    return glm::ivec2{xpos, ypos};
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::setMousePosition(const glm::ivec2 pos)
{
#ifdef CUBOS_CORE_GLFW
    glfwSetCursorPos(mHandle, (double)pos.x, (double)pos.y);
#else
    UNSUPPORTED();
#endif
}

std::shared_ptr<Cursor> GLFWWindow::createCursor(Cursor::Standard standard)
{
#ifdef CUBOS_CORE_GLFW
    GLFWcursor* cursor;
    switch (standard)
    {
    case Cursor::Standard::Arrow:
        cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        break;
    case Cursor::Standard::IBeam:
        cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        break;
    case Cursor::Standard::Cross:
        cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        break;
    case Cursor::Standard::Hand:
        cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        break;
    case Cursor::Standard::EWResize:
        cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        break;
    case Cursor::Standard::NSResize:
        cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        break;
    default:
        return nullptr;
    }

    return std::shared_ptr<Cursor>(new Cursor(cursor));
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::cursor(std::shared_ptr<Cursor> cursor)
{
#ifdef CUBOS_CORE_GLFW
    if (cursor == nullptr)
    {
        glfwSetCursor(mHandle, nullptr);
    }
    else
    {
        glfwSetCursor(mHandle, cursor->mGlfwHandle);
    }
    mCursor = cursor;
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::clipboard(const std::string& text)
{
#ifdef CUBOS_CORE_GLFW
    glfwSetClipboardString(mHandle, text.c_str());
#else
    UNSUPPORTED();
#endif
}

const char* GLFWWindow::clipboard() const
{
#ifdef CUBOS_CORE_GLFW
    return glfwGetClipboardString(mHandle);
#else
    UNSUPPORTED();
#endif
}

bool GLFWWindow::pressed(Key key, Modifiers modifiers) const
{
#ifdef CUBOS_CORE_GLFW
    // Returns true even if a superset of modifiers is active.

    return glfwGetKey(mHandle, cubosToGlfwKey(key)) == GLFW_PRESS && (modifiers & this->modifiers()) == modifiers;
#else
    UNSUPPORTED();
#endif
}

bool GLFWWindow::pressed(MouseButton button) const
{
#ifdef CUBOS_CORE_GLFW
    return glfwGetMouseButton(mHandle, cubosToGlfwMouseButton(button)) == GLFW_PRESS;
#else
    UNSUPPORTED();
#endif
}

Modifiers GLFWWindow::modifiers() const
{
#ifdef CUBOS_CORE_GLFW
    return mModifiers;
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::modifiers(Modifiers modifiers)
{
    mModifiers = modifiers;
}

bool GLFWWindow::gamepadState(int gamepad, GamepadState& state) const
{
#ifdef CUBOS_CORE_GLFW
    GLFWgamepadstate glfwState;
    if (glfwGetGamepadState(gamepad, &glfwState) == GLFW_FALSE)
    {
        return false;
    }

#define SET_BUTTON(cubos, glfw)                                                                                        \
    state.buttons[static_cast<int>(GamepadButton::cubos)] = glfwState.buttons[glfw] == GLFW_PRESS;
    SET_BUTTON(A, GLFW_GAMEPAD_BUTTON_A);
    SET_BUTTON(B, GLFW_GAMEPAD_BUTTON_B);
    SET_BUTTON(X, GLFW_GAMEPAD_BUTTON_X);
    SET_BUTTON(Y, GLFW_GAMEPAD_BUTTON_Y);
    SET_BUTTON(LBumper, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
    SET_BUTTON(RBumper, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);
    SET_BUTTON(Back, GLFW_GAMEPAD_BUTTON_BACK);
    SET_BUTTON(Start, GLFW_GAMEPAD_BUTTON_START);
    SET_BUTTON(Guide, GLFW_GAMEPAD_BUTTON_GUIDE);
    SET_BUTTON(LThumb, GLFW_GAMEPAD_BUTTON_LEFT_THUMB);
    SET_BUTTON(RThumb, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);
    SET_BUTTON(Up, GLFW_GAMEPAD_BUTTON_DPAD_UP);
    SET_BUTTON(Right, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);
    SET_BUTTON(Down, GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
    SET_BUTTON(Left, GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
#undef SET_BUTTON

#define SET_AXIS(cubos, glfw) state.axes[static_cast<int>(GamepadAxis::cubos)] = glfwState.axes[glfw];
    SET_AXIS(LX, GLFW_GAMEPAD_AXIS_LEFT_X);
    SET_AXIS(LY, GLFW_GAMEPAD_AXIS_LEFT_Y);
    SET_AXIS(RX, GLFW_GAMEPAD_AXIS_RIGHT_X);
    SET_AXIS(RY, GLFW_GAMEPAD_AXIS_RIGHT_Y);
    SET_AXIS(LTrigger, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER);
    SET_AXIS(RTrigger, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER);
#undef SET_AXIS

    return true;
#else
    UNSUPPORTED();
#endif
}

#ifdef CUBOS_CORE_GLFW

static void keyCallback(GLFWwindow* window, int key, int /*unused*/, int action, int mods)
{
    auto* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    Key cubosKey = glfwToCubosKey(key);
    updateMods(handler, mods);
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        handler->pushEvent(KeyEvent{.key = cubosKey, .pressed = action == GLFW_PRESS});
    }
}

static void mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    auto* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->pushEvent(MouseMoveEvent{.position = glm::ivec2(xPos, yPos)});
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    MouseButton cubosButton = glfwToCubosMouseButton(button);
    updateMods(handler, mods);
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        handler->pushEvent(MouseButtonEvent{.button = cubosButton, .pressed = action == GLFW_PRESS});
    }
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->pushEvent(MouseScrollEvent{.offset = glm::vec2(xoffset, yoffset)});
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->pushEvent(ResizeEvent{.size = glm::ivec2(width, height)});
}

static void charCallback(GLFWwindow* window, unsigned int codepoint)
{
    auto* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->pushEvent(TextEvent{.codepoint = static_cast<char32_t>(codepoint)});
}

static void joystickCallback(int id, int event)
{
    CUBOS_ASSERT(currentWindow != nullptr);
    currentWindow->pushEvent(GamepadConnectionEvent{
        .gamepad = id,
        .connected = event == GLFW_CONNECTED,
    });
}

static void updateMods(GLFWWindow* handler, int glfwMods)
{
    Modifiers mods = Modifiers::None;
    if ((glfwMods & GLFW_MOD_CONTROL) != 0)
    {
        mods |= Modifiers::Control;
    }
    if ((glfwMods & GLFW_MOD_SHIFT) != 0)
    {
        mods |= Modifiers::Shift;
    }
    if ((glfwMods & GLFW_MOD_ALT) != 0)
    {
        mods |= Modifiers::Alt;
    }
    if ((glfwMods & GLFW_MOD_SUPER) != 0)
    {
        mods |= Modifiers::System;
    }
    handler->modifiers(mods);
    handler->pushEvent(ModifiersEvent{.modifiers = mods});
}

static MouseButton glfwToCubosMouseButton(int button)
{
#define MAP_BUTTON(glfw, cubos)                                                                                        \
    case GLFW_MOUSE_BUTTON_##glfw:                                                                                     \
        return MouseButton::cubos;
    switch (button)
    {
        MAP_BUTTON(LEFT, Left)
        MAP_BUTTON(RIGHT, Right)
        MAP_BUTTON(MIDDLE, Middle)
        MAP_BUTTON(4, Extra1)
        MAP_BUTTON(5, Extra2)
    default:
        return MouseButton::Invalid;
    }
#undef MAP_BUTTON
}

static int cubosToGlfwMouseButton(MouseButton button)
{
#define MAP_BUTTON(cubos, glfw)                                                                                        \
    case MouseButton::cubos:                                                                                           \
        return GLFW_MOUSE_BUTTON_##glfw;
    switch (button)
    {
        MAP_BUTTON(Left, LEFT)
        MAP_BUTTON(Right, RIGHT)
        MAP_BUTTON(Middle, MIDDLE)
        MAP_BUTTON(Extra1, 4)
        MAP_BUTTON(Extra2, 5)
    default:
        return GLFW_MOUSE_BUTTON_LAST;
    }
#undef MAP_BUTTON
}

static Key glfwToCubosKey(int key)
{
#define MAP_KEY(glfw, cubos)                                                                                           \
    case GLFW_KEY_##glfw:                                                                                              \
        return Key::cubos;
    switch (key)
    {
        MAP_KEY(A, A)
        MAP_KEY(B, B)
        MAP_KEY(C, C)
        MAP_KEY(D, D)
        MAP_KEY(E, E)
        MAP_KEY(F, F)
        MAP_KEY(G, G)
        MAP_KEY(H, H)
        MAP_KEY(I, I)
        MAP_KEY(J, J)
        MAP_KEY(K, K)
        MAP_KEY(L, L)
        MAP_KEY(M, M)
        MAP_KEY(N, N)
        MAP_KEY(O, O)
        MAP_KEY(P, P)
        MAP_KEY(Q, Q)
        MAP_KEY(R, R)
        MAP_KEY(S, S)
        MAP_KEY(T, T)
        MAP_KEY(U, U)
        MAP_KEY(V, V)
        MAP_KEY(W, W)
        MAP_KEY(X, X)
        MAP_KEY(Y, Y)
        MAP_KEY(Z, Z)
        MAP_KEY(0, Num0)
        MAP_KEY(1, Num1)
        MAP_KEY(2, Num2)
        MAP_KEY(3, Num3)
        MAP_KEY(4, Num4)
        MAP_KEY(5, Num5)
        MAP_KEY(6, Num6)
        MAP_KEY(7, Num7)
        MAP_KEY(8, Num8)
        MAP_KEY(9, Num9)
        MAP_KEY(ESCAPE, Escape)
        MAP_KEY(LEFT_CONTROL, LControl)
        MAP_KEY(LEFT_SHIFT, LShift)
        MAP_KEY(LEFT_ALT, LAlt)
        MAP_KEY(LEFT_SUPER, LSystem)
        MAP_KEY(RIGHT_CONTROL, RControl)
        MAP_KEY(RIGHT_SHIFT, RShift)
        MAP_KEY(RIGHT_ALT, RAlt)
        MAP_KEY(RIGHT_SUPER, RSystem)
        MAP_KEY(MENU, Menu)
        MAP_KEY(LEFT_BRACKET, LBracket)
        MAP_KEY(RIGHT_BRACKET, RBracket)
        MAP_KEY(SEMICOLON, SemiColon)
        MAP_KEY(COMMA, Comma)
        MAP_KEY(PERIOD, Period)
        MAP_KEY(APOSTROPHE, Quote)
        MAP_KEY(SLASH, Slash)
        MAP_KEY(BACKSLASH, BackSlash)
        MAP_KEY(EQUAL, Equal)
        MAP_KEY(MINUS, Dash)
        MAP_KEY(SPACE, Space)
        MAP_KEY(ENTER, Return)
        MAP_KEY(BACKSPACE, BackSpace)
        MAP_KEY(TAB, Tab)
        MAP_KEY(PAGE_UP, PageUp)
        MAP_KEY(PAGE_DOWN, PageDown)
        MAP_KEY(END, End)
        MAP_KEY(HOME, Home)
        MAP_KEY(INSERT, Insert)
        MAP_KEY(DELETE, Delete)
        MAP_KEY(KP_ADD, Add)
        MAP_KEY(KP_SUBTRACT, Subtract)
        MAP_KEY(KP_MULTIPLY, Multiply)
        MAP_KEY(KP_DIVIDE, Divide)
        MAP_KEY(LEFT, Left)
        MAP_KEY(RIGHT, Right)
        MAP_KEY(UP, Up)
        MAP_KEY(DOWN, Down)
        MAP_KEY(KP_0, Numpad0)
        MAP_KEY(KP_1, Numpad1)
        MAP_KEY(KP_2, Numpad2)
        MAP_KEY(KP_3, Numpad3)
        MAP_KEY(KP_4, Numpad4)
        MAP_KEY(KP_5, Numpad5)
        MAP_KEY(KP_6, Numpad6)
        MAP_KEY(KP_7, Numpad7)
        MAP_KEY(KP_8, Numpad8)
        MAP_KEY(KP_9, Numpad9)
        MAP_KEY(F1, F1)
        MAP_KEY(F2, F2)
        MAP_KEY(F3, F3)
        MAP_KEY(F4, F4)
        MAP_KEY(F5, F5)
        MAP_KEY(F6, F6)
        MAP_KEY(F7, F7)
        MAP_KEY(F8, F8)
        MAP_KEY(F9, F9)
        MAP_KEY(F10, F10)
        MAP_KEY(F11, F11)
        MAP_KEY(F12, F12)
        MAP_KEY(PAUSE, Pause)
    default:
        return Key::Invalid;
    }
#undef MAP_KEY
}

static int cubosToGlfwKey(Key key)
{
#define MAP_KEY(cubos, glfw)                                                                                           \
    case Key::cubos:                                                                                                   \
        return GLFW_KEY_##glfw;
    switch (key)
    {
        MAP_KEY(A, A)
        MAP_KEY(B, B)
        MAP_KEY(C, C)
        MAP_KEY(D, D)
        MAP_KEY(E, E)
        MAP_KEY(F, F)
        MAP_KEY(G, G)
        MAP_KEY(H, H)
        MAP_KEY(I, I)
        MAP_KEY(J, J)
        MAP_KEY(K, K)
        MAP_KEY(L, L)
        MAP_KEY(M, M)
        MAP_KEY(N, N)
        MAP_KEY(O, O)
        MAP_KEY(P, P)
        MAP_KEY(Q, Q)
        MAP_KEY(R, R)
        MAP_KEY(S, S)
        MAP_KEY(T, T)
        MAP_KEY(U, U)
        MAP_KEY(V, V)
        MAP_KEY(W, W)
        MAP_KEY(X, X)
        MAP_KEY(Y, Y)
        MAP_KEY(Z, Z)
        MAP_KEY(Num0, 0)
        MAP_KEY(Num1, 1)
        MAP_KEY(Num2, 2)
        MAP_KEY(Num3, 3)
        MAP_KEY(Num4, 4)
        MAP_KEY(Num5, 5)
        MAP_KEY(Num6, 6)
        MAP_KEY(Num7, 7)
        MAP_KEY(Num8, 8)
        MAP_KEY(Num9, 9)
        MAP_KEY(Escape, ESCAPE)
        MAP_KEY(LControl, LEFT_CONTROL)
        MAP_KEY(LShift, LEFT_SHIFT)
        MAP_KEY(LAlt, LEFT_ALT)
        MAP_KEY(LSystem, LEFT_SUPER)
        MAP_KEY(RControl, RIGHT_CONTROL)
        MAP_KEY(RShift, RIGHT_SHIFT)
        MAP_KEY(RAlt, RIGHT_ALT)
        MAP_KEY(RSystem, RIGHT_SUPER)
        MAP_KEY(Menu, MENU)
        MAP_KEY(LBracket, LEFT_BRACKET)
        MAP_KEY(RBracket, RIGHT_BRACKET)
        MAP_KEY(SemiColon, SEMICOLON)
        MAP_KEY(Comma, COMMA)
        MAP_KEY(Period, PERIOD)
        MAP_KEY(Quote, APOSTROPHE)
        MAP_KEY(Slash, SLASH)
        MAP_KEY(BackSlash, BACKSLASH)
        MAP_KEY(Equal, EQUAL)
        MAP_KEY(Dash, MINUS)
        MAP_KEY(Space, SPACE)
        MAP_KEY(Return, ENTER)
        MAP_KEY(BackSpace, BACKSPACE)
        MAP_KEY(Tab, TAB)
        MAP_KEY(PageUp, PAGE_UP)
        MAP_KEY(PageDown, PAGE_DOWN)
        MAP_KEY(End, END)
        MAP_KEY(Home, HOME)
        MAP_KEY(Insert, INSERT)
        MAP_KEY(Delete, DELETE)
        MAP_KEY(Add, KP_ADD)
        MAP_KEY(Subtract, KP_SUBTRACT)
        MAP_KEY(Multiply, KP_MULTIPLY)
        MAP_KEY(Divide, KP_DIVIDE)
        MAP_KEY(Left, LEFT)
        MAP_KEY(Right, RIGHT)
        MAP_KEY(Up, UP)
        MAP_KEY(Down, DOWN)
        MAP_KEY(Numpad0, KP_0)
        MAP_KEY(Numpad1, KP_1)
        MAP_KEY(Numpad2, KP_2)
        MAP_KEY(Numpad3, KP_3)
        MAP_KEY(Numpad4, KP_4)
        MAP_KEY(Numpad5, KP_5)
        MAP_KEY(Numpad6, KP_6)
        MAP_KEY(Numpad7, KP_7)
        MAP_KEY(Numpad8, KP_8)
        MAP_KEY(Numpad9, KP_9)
        MAP_KEY(F1, F1)
        MAP_KEY(F2, F2)
        MAP_KEY(F3, F3)
        MAP_KEY(F4, F4)
        MAP_KEY(F5, F5)
        MAP_KEY(F6, F6)
        MAP_KEY(F7, F7)
        MAP_KEY(F8, F8)
        MAP_KEY(F9, F9)
        MAP_KEY(F10, F10)
        MAP_KEY(F11, F11)
        MAP_KEY(F12, F12)
        MAP_KEY(Pause, PAUSE)
    default:
        return GLFW_KEY_UNKNOWN;
    }
#undef MAP_KEY
}

#endif // CUBOS_CORE_GLFW
