#include <cubos/core/gl/imgui_impl.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;

struct ImGui_ImplCubos_Data
{
    io::Window& window;
    gl::RenderDevice& renderDevice;

    size_t onMouseMovedId;
    size_t onMouseDownId;
    size_t onMouseUpId;
    size_t onMouseScrollId;
    size_t onKeyDownId;
    size_t onKeyUpId;

    gl::Texture2D texture;
};

static int ImGui_ImplCubos_ButtonToImGuiButton(io::MouseButton button)
{
    switch (button)
    {
    case io::MouseButton::Left:
        return ImGuiMouseButton_Left;
    case io::MouseButton::Right:
        return ImGuiMouseButton_Right;
    case io::MouseButton::Middle:
        return ImGuiMouseButton_Middle;
    default:
        return -1;
    }
}

static int ImGui_ImplCubos_KeyToImGuiKey(io::Key key)
{
#define MAP_KEY(cubos, imgui)                                                                                          \
    case io::Key::cubos:                                                                                               \
        return ImGuiKey_##imgui;
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
        MAP_KEY(Escape, Escape)
        MAP_KEY(LControl, LeftCtrl)
        MAP_KEY(LShift, LeftShift)
        MAP_KEY(LAlt, LeftAlt)
        MAP_KEY(LSystem, LeftSuper)
        MAP_KEY(RControl, RightCtrl)
        MAP_KEY(RShift, RightShift)
        MAP_KEY(RAlt, RightAlt)
        MAP_KEY(RSystem, RightSuper)
        MAP_KEY(Menu, Menu)
        MAP_KEY(LBracket, LeftBracket)
        MAP_KEY(RBracket, RightBracket)
        MAP_KEY(SemiColon, Semicolon)
        MAP_KEY(Comma, Comma)
        MAP_KEY(Period, Period)
        MAP_KEY(Quote, Apostrophe)
        MAP_KEY(Slash, Slash)
        MAP_KEY(BackSlash, Backslash)
        MAP_KEY(Equal, Equal)
        MAP_KEY(Dash, Minus)
        MAP_KEY(Space, Space)
        MAP_KEY(Return, Enter)
        MAP_KEY(BackSpace, Backspace)
        MAP_KEY(Tab, Tab)
        MAP_KEY(PageUp, PageUp)
        MAP_KEY(PageDown, PageDown)
        MAP_KEY(End, End)
        MAP_KEY(Home, Home)
        MAP_KEY(Insert, Insert)
        MAP_KEY(Delete, Delete)
        MAP_KEY(Add, KeypadAdd)
        MAP_KEY(Subtract, KeypadSubtract)
        MAP_KEY(Multiply, KeypadMultiply)
        MAP_KEY(Divide, KeypadDivide)
        MAP_KEY(Left, LeftArrow)
        MAP_KEY(Right, RightArrow)
        MAP_KEY(Up, UpArrow)
        MAP_KEY(Down, DownArrow)
        MAP_KEY(Numpad0, Keypad0)
        MAP_KEY(Numpad1, Keypad1)
        MAP_KEY(Numpad2, Keypad2)
        MAP_KEY(Numpad3, Keypad3)
        MAP_KEY(Numpad4, Keypad4)
        MAP_KEY(Numpad5, Keypad5)
        MAP_KEY(Numpad6, Keypad6)
        MAP_KEY(Numpad7, Keypad7)
        MAP_KEY(Numpad8, Keypad8)
        MAP_KEY(Numpad9, Keypad9)
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
        MAP_KEY(Pause, Pause)
    default:
        return ImGuiKey_None;
    }
#undef MAP_KEY
}

static bool ImGui_ImplCubos_CreateDeviceObjects()
{
    // Build texture atlas.
    auto& io = ImGui::GetIO();
    auto bd = (ImGui_ImplCubos_Data*)io.UserData;

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to GPU.
    gl::Texture2DDesc desc;
    desc.data[0] = pixels;
    desc.width = static_cast<size_t>(width);
    desc.height = static_cast<size_t>(height);
    desc.mipLevelCount = 1;
    desc.format = gl::TextureFormat::RGBA8UNorm;
    desc.usage = gl::Usage::Static;
    bd->texture = bd->renderDevice.createTexture2D(desc);
    if (!bd->texture)
        return false;
    io.Fonts->SetTexID((void*)bd->texture.get());
}

bool ImGui_ImplCubos_Init(io::Window& window)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.BackendPlatformUserData != nullptr)
    {
        logError("ImGui_ImplCubos_Init: already initialized");
        return false;
    }

    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    // Setup back-end capabilities flags
    ImGui_ImplCubos_Data* bd = IM_NEW(ImGui_ImplCubos_Data){window, window.getRenderDevice()};
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_cubos";

    bd->onMouseMovedId = window.onMouseMoved.registerCallback([](glm::ivec2 pos) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(static_cast<float>(pos.x), static_cast<float>(pos.y));
    });

    bd->onMouseDownId = window.onMouseDown.registerCallback([](io::MouseButton button) {
        ImGuiIO& io = ImGui::GetIO();
        int b = ImGui_ImplCubos_ButtonToImGuiButton(button);
        if (b != -1)
            io.AddMouseButtonEvent(b, true);
    });

    bd->onMouseUpId = window.onMouseUp.registerCallback([](io::MouseButton button) {
        ImGuiIO& io = ImGui::GetIO();
        int b = ImGui_ImplCubos_ButtonToImGuiButton(button);
        if (b != -1)
            io.AddMouseButtonEvent(b, false);
    });

    bd->onMouseScrollId = window.onMouseScroll.registerCallback([](glm::ivec2 scroll) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(static_cast<float>(scroll.x), static_cast<float>(scroll.y));
    });

    bd->onKeyDownId = window.onKeyDown.registerCallback([](io::Key key) {
        ImGuiIO& io = ImGui::GetIO();
        int k = ImGui_ImplCubos_KeyToImGuiKey(key);
        if (k != -1)
            io.AddKeyEvent(k, true);
    });

    bd->onKeyUpId = window.onKeyUp.registerCallback([](io::Key key) {
        ImGuiIO& io = ImGui::GetIO();
        int k = ImGui_ImplCubos_KeyToImGuiKey(key);
        if (k != -1)
            io.AddKeyEvent(k, false);
    });

    return true;
}

void ImGui_ImplCubos_Shutdown()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData != nullptr);
    ImGui_ImplCubos_Data* bd = (ImGui_ImplCubos_Data*)io.BackendPlatformUserData;

    bd->window.onMouseMoved.unregisterCallback(bd->onMouseMovedId);
    bd->window.onMouseDown.unregisterCallback(bd->onMouseDownId);
    bd->window.onMouseUp.unregisterCallback(bd->onMouseUpId);
    bd->window.onMouseScroll.unregisterCallback(bd->onMouseScrollId);
    bd->window.onKeyDown.unregisterCallback(bd->onKeyDownId);
    bd->window.onKeyUp.unregisterCallback(bd->onKeyUpId);

    IM_DELETE(bd);
    io.BackendPlatformUserData = nullptr;
}

void ImGui_ImplCubos_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplCubos_Data* bd = (ImGui_ImplCubos_Data*)io.BackendPlatformUserData;
    io.DisplaySize.x = bd->window.getFramebufferSize().x;
    io.DisplaySize.y = bd->window.getFramebufferSize().y;
}

void ImGui_ImplCubos_RenderDrawData(ImDrawData* drawData)
{
    // Render command lists.
    for (int n = 0; n < drawData->CmdListsCount; ++n)
    {
        const auto* cmdList = drawData->CmdLists[n];


    }
}
