#include <cubos/core/io/glfw_window.hpp>

#include <cubos/core/gl/ogl_render_device.hpp>

#include <cubos/core/log.hpp>

#include <glad/glad.h>

using namespace cubos::core;
using namespace cubos::core::io;

#ifdef WITH_GLFW

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void charCallback(GLFWwindow* window, unsigned int codepoint);
static void updateMods(GLFWWindow* handler, int mods);
static MouseButton glfwToCubosMouseButton(int button);
static Key glfwToCubosKey(int key);

#endif // WITH_GLFW

#define UNSUPPORTED()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Unsupported when building without GLFW");                                                      \
        abort();                                                                                                       \
    } while (0)

GLFWWindow::GLFWWindow()
{
#ifdef WITH_GLFW
    if (!glfwInit())
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
    // TODO: Read settings and set the correct window size, title and mode (fullscreen, ...)
    this->handle = glfwCreateWindow(800, 600, "Cubos", nullptr, nullptr);
    if (!this->handle)
    {
        CUBOS_CRITICAL("glfwCreateWindow() failed");
        abort();
    }

    // Create OpenGL render device
    glfwMakeContextCurrent(this->handle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CUBOS_CRITICAL("OpenGL loader failed");
        abort();
    }
    this->renderDevice = new gl::OGLRenderDevice();

    // Set input callbacks
    glfwSetWindowUserPointer(this->handle, (void*)this);
    glfwSetKeyCallback(this->handle, keyCallback);
    glfwSetCursorPosCallback(this->handle, mousePositionCallback);
    glfwSetMouseButtonCallback(this->handle, mouseButtonCallback);
    glfwSetScrollCallback(this->handle, scrollCallback);
    glfwSetFramebufferSizeCallback(this->handle, framebufferSizeCallback);
    glfwSetCharCallback(this->handle, charCallback);
#else
    UNSUPPORTED();
#endif
}

GLFWWindow::~GLFWWindow()
{
#ifdef WITH_GLFW
    delete this->renderDevice;
    glfwTerminate();
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::pollEvents() const
{
#ifdef WITH_GLFW
    glfwPollEvents();
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::swapBuffers() const
{
#ifdef WITH_GLFW
    glfwSwapBuffers(this->handle);
#else
    UNSUPPORTED();
#endif
}

gl::RenderDevice& GLFWWindow::getRenderDevice() const
{
#ifdef WITH_GLFW
    return *this->renderDevice;
#else
    UNSUPPORTED();
#endif
}

glm::ivec2 GLFWWindow::getSize() const
{
#ifdef WITH_GLFW
    int width, height;
    glfwGetWindowSize(this->handle, &width, &height);
    return {width, height};
#else
    UNSUPPORTED();
#endif
}

glm::ivec2 GLFWWindow::getFramebufferSize() const
{
#ifdef WITH_GLFW
    int width, height;
    glfwGetFramebufferSize(this->handle, &width, &height);
    return {width, height};
#else
    UNSUPPORTED();
#endif
}

bool GLFWWindow::shouldClose() const
{
#ifdef WITH_GLFW
    return glfwWindowShouldClose(this->handle);
#else
    UNSUPPORTED();
#endif
}

double GLFWWindow::getTime() const
{
#ifdef WITH_GLFW
    return glfwGetTime();
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::setMouseState(MouseState state)
{
#ifdef WITH_GLFW
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
    }
    glfwSetInputMode(handle, GLFW_CURSOR, cursorState);
#else
    UNSUPPORTED();
#endif
}

MouseState GLFWWindow::getMouseState() const
{
#ifdef WITH_GLFW
    switch (glfwGetInputMode(handle, GLFW_CURSOR))
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

std::shared_ptr<Cursor> GLFWWindow::createCursor(Cursor::Standard standard)
{
#ifdef WITH_GLFW
    GLFWcursor* cursor = nullptr;
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
    }

    if (cursor == nullptr)
        return nullptr;
    else
        return std::shared_ptr<Cursor>(new Cursor(cursor));
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::setCursor(std::shared_ptr<Cursor> cursor)
{
#ifdef WITH_GLFW
    if (cursor == nullptr)
        glfwSetCursor(this->handle, nullptr);
    else
        glfwSetCursor(this->handle, cursor->glfwHandle);
    this->cursor = cursor;
#else
    UNSUPPORTED();
#endif
}

void GLFWWindow::setClipboard(const std::string& text)
{
#ifdef WITH_GLFW
    glfwSetClipboardString(this->handle, text.c_str());
#else
    UNSUPPORTED();
#endif
}

const char* GLFWWindow::getClipboard() const
{
#ifdef WITH_GLFW
    return glfwGetClipboardString(this->handle);
#else
    UNSUPPORTED();
#endif
}

#ifdef WITH_GLFW

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    Key cubosKey = glfwToCubosKey(key);
    updateMods(handler, mods);
    if (action == GLFW_PRESS)
    {
        handler->onKeyDown.fire(cubosKey);
    }
    else if (action == GLFW_RELEASE)
    {
        handler->onKeyUp.fire(cubosKey);
    }
}

static void mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->onMouseMoved.fire(glm::ivec2(xPos, yPos));
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    MouseButton cubosButton = glfwToCubosMouseButton(button);
    updateMods(handler, mods);
    if (action == GLFW_PRESS)
        handler->onMouseDown.fire(cubosButton);
    else // with GLFW_RELEASE
        handler->onMouseUp.fire(cubosButton);
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->onMouseScroll.fire(glm::ivec2(xoffset, yoffset));
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->onFramebufferResize.fire(glm::ivec2(width, height));
}

static void charCallback(GLFWwindow* window, unsigned int codepoint)
{
    GLFWWindow* handler = (GLFWWindow*)glfwGetWindowUserPointer(window);
    handler->onChar.fire(codepoint);
}

static void updateMods(GLFWWindow* handler, int glfwMods)
{
    Modifiers mods = Modifiers::None;
    if (glfwMods & GLFW_MOD_CONTROL)
        mods |= Modifiers::Control;
    if (glfwMods & GLFW_MOD_SHIFT)
        mods |= Modifiers::Shift;
    if (glfwMods & GLFW_MOD_ALT)
        mods |= Modifiers::Alt;
    if (glfwMods & GLFW_MOD_SUPER)
        mods |= Modifiers::System;
    handler->onModsChanged.fire(mods);
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

#endif // WITH_GLFW
