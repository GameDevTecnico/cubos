#include <array>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <implot.h>

#include <cubos/core/io/cursor.hpp>
#include <cubos/core/tel/logging.hpp>

#include "imgui.hpp"

namespace gl = cubos::core::gl;
namespace io = cubos::core::io;

struct ImGuiData
{
    io::Window window;

    std::shared_ptr<io::Cursor> cursors[ImGuiMouseCursor_COUNT];

    double time;

    gl::RasterState rasterState;
    gl::BlendState blendState;
    gl::DepthStencilState depthStencilState;
    gl::ShaderPipeline pipeline;
    gl::Texture2D texture;
    gl::ConstantBuffer cb;
    gl::VertexArray va;
    gl::VertexBuffer vb;
    gl::IndexBuffer ib;
    std::size_t vbSize, ibSize;

    gl::ShaderBindingPoint textureBP;
    gl::ShaderBindingPoint cbBP;
};

static int buttonToImGuiButton(io::MouseButton button)
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

static ImGuiKey keyToImGuiKey(io::Key key)
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

static void setClipboardText(void* userData, const char* text)
{
    auto* bd = static_cast<ImGuiData*>(userData);
    bd->window->clipboard(text);
}

static const char* getClipboardText(void* userData)
{
    auto* bd = static_cast<ImGuiData*>(userData);
    return bd->window->clipboard();
}

static void createDeviceObjects()
{
    auto& io = ImGui::GetIO();
    auto* bd = (ImGuiData*)io.BackendPlatformUserData;
    auto& rd = bd->window->renderDevice();

    // Setup render states.

    gl::RasterStateDesc rasterStateDesc;
    rasterStateDesc.cullEnabled = false;
    rasterStateDesc.scissorEnabled = true;
    bd->rasterState = rd.createRasterState(rasterStateDesc);

    gl::BlendStateDesc blendStateDesc;
    blendStateDesc.blendEnabled = true;
    blendStateDesc.color.src = gl::BlendFactor::SrcAlpha;
    blendStateDesc.color.dst = gl::BlendFactor::InvSrcAlpha;
    blendStateDesc.color.op = gl::BlendOp::Add;
    blendStateDesc.alpha.src = gl::BlendFactor::One;
    blendStateDesc.alpha.dst = gl::BlendFactor::InvSrcAlpha;
    blendStateDesc.alpha.op = gl::BlendOp::Add;
    bd->blendState = rd.createBlendState(blendStateDesc);

    gl::DepthStencilStateDesc depthStencilStateDesc;
    depthStencilStateDesc.depth.enabled = false;
    depthStencilStateDesc.stencil.enabled = false;
    bd->depthStencilState = rd.createDepthStencilState(depthStencilStateDesc);

    // Setup shader pipeline.
    auto vs = rd.createShaderStage(gl::Stage::Vertex, R"glsl(
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

uniform Proj {
    mat4 proj;
};

out vec2 fragUv;
out vec4 fragColor;

void main()
{
    fragUv = uv;
    fragColor = color;
    gl_Position = proj * vec4(position, 0.0, 1.0);
}
    )glsl");

    auto ps = rd.createShaderStage(gl::Stage::Pixel, R"glsl(
uniform sampler2D tex;

in vec2 fragUv;
in vec4 fragColor;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = fragColor * texture(tex, fragUv);
}
    )glsl");

    bd->pipeline = rd.createShaderPipeline(vs, ps);
    bd->textureBP = bd->pipeline->getBindingPoint("tex");
    bd->cbBP = bd->pipeline->getBindingPoint("Proj");

    // Build texture atlas.
    unsigned char* pixels;
    int width;
    int height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to GPU.
    gl::Texture2DDesc desc;
    desc.data[0] = pixels;
    desc.width = static_cast<std::size_t>(width);
    desc.height = static_cast<std::size_t>(height);
    desc.mipLevelCount = 1;
    desc.format = gl::TextureFormat::RGBA8UNorm;
    desc.usage = gl::Usage::Static;
    bd->texture = rd.createTexture2D(desc);
    if (!bd->texture)
    {
        CUBOS_CRITICAL("Failed to create ImGui font texture");
        abort();
    }
    io.Fonts->SetTexID((void*)bd->texture.get());

    // Create projection constant buffer.
    bd->cb = rd.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);

    // Initialize buffer sizes.
    bd->vbSize = 0;
    bd->ibSize = 0;
}

void cubos::engine::imguiInitialize(const io::Window& window, float dpiScale)
{
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    if (io.BackendPlatformUserData != nullptr)
    {
        CUBOS_WARN("Already initialized");
        return;
    }

    // Setup back-end capabilities flags
    auto* bd = IM_NEW(ImGuiData)();
    bd->window = window;
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_cubos";

    // Create mouse cursors.
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    bd->cursors[ImGuiMouseCursor_Arrow] = window->createCursor(io::Cursor::Standard::Arrow);
    bd->cursors[ImGuiMouseCursor_TextInput] = window->createCursor(io::Cursor::Standard::IBeam);
    bd->cursors[ImGuiMouseCursor_ResizeAll] = window->createCursor(io::Cursor::Standard::AllResize);
    bd->cursors[ImGuiMouseCursor_ResizeNS] = window->createCursor(io::Cursor::Standard::NSResize);
    bd->cursors[ImGuiMouseCursor_ResizeEW] = window->createCursor(io::Cursor::Standard::EWResize);
    bd->cursors[ImGuiMouseCursor_ResizeNESW] = window->createCursor(io::Cursor::Standard::NESWResize);
    bd->cursors[ImGuiMouseCursor_ResizeNWSE] = window->createCursor(io::Cursor::Standard::NWSEResize);
    bd->cursors[ImGuiMouseCursor_Hand] = window->createCursor(io::Cursor::Standard::Hand);
    bd->cursors[ImGuiMouseCursor_NotAllowed] = window->createCursor(io::Cursor::Standard::NotAllowed);

    for (auto& cursor : bd->cursors)
    {
        if (cursor == nullptr)
        {
            cursor = bd->cursors[ImGuiMouseCursor_Arrow];
            ;
        }
    }

    io.SetClipboardTextFn = setClipboardText;
    io.GetClipboardTextFn = getClipboardText;
    io.ClipboardUserData = static_cast<void*>(bd);

    bd->time = window->time();

    // DPI scaling
    if (dpiScale < 1.0F)
    {
        // Smaller sizes are possible but may cause crashes.
        // Anything <= 0.0f is invalid.
        dpiScale = 1.0F;
    }
    ImGui::GetStyle().ScaleAllSizes(dpiScale);
    ImFontConfig fontCfg;
    // Default font size is 13; scale that
    fontCfg.SizePixels = 13.0F * dpiScale;
    io.Fonts->AddFontDefault(&fontCfg);

    createDeviceObjects();

    CUBOS_INFO("Initialized UI");
}

void cubos::engine::imguiTerminate()
{
    auto& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData != nullptr);
    auto* bd = (ImGuiData*)io.BackendPlatformUserData;

    for (auto& cursor : bd->cursors)
    {
        cursor = nullptr;
    }

    bd->rasterState = nullptr;
    bd->blendState = nullptr;
    bd->depthStencilState = nullptr;
    bd->texture = nullptr;
    bd->pipeline = nullptr;
    bd->textureBP = nullptr;
    bd->cbBP = nullptr;
    bd->cb = nullptr;
    bd->vb = nullptr;
    bd->ib = nullptr;
    bd->va = nullptr;

    IM_DELETE(bd);
    io.BackendPlatformUserData = nullptr;

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void cubos::engine::imguiBeginFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    auto* bd = (ImGuiData*)io.BackendPlatformUserData;
    io.DisplaySize.x = static_cast<float>(bd->window->framebufferSize().x);
    io.DisplaySize.y = static_cast<float>(bd->window->framebufferSize().y);
    io.DisplayFramebufferScale.x = io.DisplaySize.x / static_cast<float>(bd->window->size().x);
    io.DisplayFramebufferScale.y = io.DisplaySize.y / static_cast<float>(bd->window->size().y);

    // Setup time step.
    double time = bd->window->time();
    io.DeltaTime = static_cast<float>(time - bd->time);
    bd->time = time;

    // Update cursor.
    if (((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0) &&
        bd->window->mouseState() != io::MouseState::Locked)
    {
        ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
        if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor.
            bd->window->mouseState(io::MouseState::Hidden);
        }
        else
        {
            // Show OS mouse cursor.
            bd->window->cursor(bd->cursors[static_cast<std::size_t>(imguiCursor)]);
            bd->window->mouseState(io::MouseState::Default);
        }
    }

    ImGui::NewFrame();
}

static void setupRenderState(ImGuiData* bd, gl::Framebuffer target)
{
    auto& rd = bd->window->renderDevice();
    rd.setRasterState(bd->rasterState);
    rd.setBlendState(bd->blendState);
    rd.setDepthStencilState(bd->depthStencilState);
    rd.setShaderPipeline(bd->pipeline);
    rd.setFramebuffer(std::move(target));
}

void cubos::engine::imguiEndFrame(const gl::Framebuffer& target)
{
    auto* bd = (ImGuiData*)ImGui::GetIO().BackendPlatformUserData;
    auto& rd = bd->window->renderDevice();
    ImGui::Render();
    auto* drawData = ImGui::GetDrawData();

    // Upload projection matrix to constant buffer.
    glm::mat4& proj = *(glm::mat4*)bd->cb->map();
    proj = glm::ortho(drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x,
                      drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y);
    bd->cb->unmap();

    // Set render state.
    setupRenderState(bd, target);
    rd.setViewport(0, 0, static_cast<int>(drawData->DisplaySize.x), static_cast<int>(drawData->DisplaySize.y));

    // Render command lists.
    ImVec2 clipOff = drawData->DisplayPos;
    for (int n = 0; n < drawData->CmdListsCount; ++n)
    {
        const auto* cmdList = drawData->CmdLists[n];

        // Create and grow vertex buffer if needed.
        if (!bd->vb || bd->vbSize < static_cast<std::size_t>(cmdList->VtxBuffer.Size))
        {
            bd->vbSize = static_cast<std::size_t>(cmdList->VtxBuffer.Size) + 5000;
            bd->vb = rd.createVertexBuffer(bd->vbSize * sizeof(ImDrawVert), nullptr, gl::Usage::Dynamic);

            // Create the vertex array.
            gl::VertexArrayDesc desc;
            desc.elementCount = 3;
            desc.elements[0].name = "position";
            desc.elements[0].type = gl::Type::Float;
            desc.elements[0].size = 2;
            desc.elements[0].buffer.index = 0;
            desc.elements[0].buffer.offset = offsetof(ImDrawVert, pos);
            desc.elements[0].buffer.stride = sizeof(ImDrawVert);
            desc.elements[1].name = "uv";
            desc.elements[1].type = gl::Type::Float;
            desc.elements[1].size = 2;
            desc.elements[1].buffer.index = 0;
            desc.elements[1].buffer.offset = offsetof(ImDrawVert, uv);
            desc.elements[1].buffer.stride = sizeof(ImDrawVert);
            desc.elements[2].name = "color";
            desc.elements[2].type = gl::Type::NUByte;
            desc.elements[2].size = 4;
            desc.elements[2].buffer.index = 0;
            desc.elements[2].buffer.offset = offsetof(ImDrawVert, col);
            desc.elements[2].buffer.stride = sizeof(ImDrawVert);
            desc.buffers[0] = bd->vb;
            desc.shaderPipeline = bd->pipeline;
            bd->va = rd.createVertexArray(desc);
        }

        // Create and grow index buffer if needed.
        if (!bd->ib || bd->ibSize < static_cast<std::size_t>(cmdList->IdxBuffer.Size))
        {
            bd->ibSize = static_cast<std::size_t>(cmdList->IdxBuffer.Size) + 10000;
            bd->ib = rd.createIndexBuffer(bd->ibSize * sizeof(ImDrawIdx), nullptr,
                                          sizeof(ImDrawIdx) == 2 ? gl::IndexFormat::UShort : gl::IndexFormat::UInt,
                                          gl::Usage::Dynamic);
        }

        // Upload vertex and index data into vertex buffer.
        auto* vtxDst = (ImDrawVert*)bd->vb->map();
        auto* idxDst = (ImDrawIdx*)bd->ib->map();
        memcpy(vtxDst, cmdList->VtxBuffer.Data, static_cast<std::size_t>(cmdList->VtxBuffer.Size) * sizeof(ImDrawVert));
        memcpy(idxDst, cmdList->IdxBuffer.Data, static_cast<std::size_t>(cmdList->IdxBuffer.Size) * sizeof(ImDrawIdx));
        bd->vb->unmap();
        bd->ib->unmap();

        rd.setVertexArray(bd->va);
        rd.setIndexBuffer(bd->ib);
        bd->textureBP->bind(bd->texture);
        bd->cbBP->bind(bd->cb);

        for (int i = 0; i < cmdList->CmdBuffer.Size; i++)
        {
            const auto* cmd = &cmdList->CmdBuffer[i];
            if (cmd->UserCallback != nullptr)
            {
                if (cmd->UserCallback == ImDrawCallback_ResetRenderState)
                {
                    setupRenderState(bd, target);
                    rd.setVertexArray(bd->va);
                    rd.setIndexBuffer(bd->ib);
                    bd->textureBP->bind(bd->texture);
                    bd->cbBP->bind(bd->cb);
                }
                else
                {
                    cmd->UserCallback(cmdList, cmd);
                }
            }
            else
            {
                // Project scissor/clipping rectangle into framebuffer space.
                glm::ivec2 clipMin = {cmd->ClipRect.x - clipOff.x, cmd->ClipRect.y - clipOff.y};
                glm::ivec2 clipMax = {cmd->ClipRect.z - clipOff.x, cmd->ClipRect.w - clipOff.y};
                if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                {
                    continue;
                }

                // Apply the scissor/clipping rectangle (with Y flipped)
                rd.setScissor(clipMin.x, static_cast<int>(drawData->DisplaySize.y) - clipMax.y, clipMax.x - clipMin.x,
                              clipMax.y - clipMin.y);

                // Bind the texture and draw.
                rd.drawTrianglesIndexed(cmd->IdxOffset, cmd->ElemCount);
            }
        }
    }
}

static bool handle(const io::MouseMoveEvent& event)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(static_cast<float>(event.position.x) * io.DisplayFramebufferScale.x,
                         static_cast<float>(event.position.y) * io.DisplayFramebufferScale.y);
    return io.WantCaptureMouse;
}

static bool handle(const io::MouseButtonEvent& event)
{
    ImGuiIO& io = ImGui::GetIO();
    int button = buttonToImGuiButton(event.button);
    if (button != -1)
    {
        io.AddMouseButtonEvent(button, event.pressed);
    }
    return io.WantCaptureMouse;
}

static bool handle(const io::MouseScrollEvent& event)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(static_cast<float>(event.offset.x), static_cast<float>(event.offset.y));
    return io.WantCaptureMouse;
}

static bool handle(const io::KeyEvent& event)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiKey key = keyToImGuiKey(event.key);
    if (key != -1)
    {
        io.AddKeyEvent(key, event.pressed);
    }
    return io.WantCaptureKeyboard;
}

static bool handle(const io::TextEvent& event)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(event.codepoint);
    return io.WantCaptureKeyboard;
}

static bool handle(const io::ModifiersEvent& event)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiKey_ModCtrl, (event.modifiers & io::Modifiers::Control) != io::Modifiers::None);
    io.AddKeyEvent(ImGuiKey_ModShift, (event.modifiers & io::Modifiers::Shift) != io::Modifiers::None);
    io.AddKeyEvent(ImGuiKey_ModAlt, (event.modifiers & io::Modifiers::Alt) != io::Modifiers::None);
    io.AddKeyEvent(ImGuiKey_ModSuper, (event.modifiers & io::Modifiers::System) != io::Modifiers::None);
    return io.WantCaptureKeyboard;
}

static bool handle(auto&& /*unused*/)
{
    return false;
}

bool cubos::engine::imguiHandleEvent(const io::WindowEvent& event)
{
    return std::visit([&](const auto& e) { return handle(e); }, event);
}
