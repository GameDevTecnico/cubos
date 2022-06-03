#include <cubos/core/ui/imgui.hpp>
#include <cubos/core/io/cursor.hpp>
#include <cubos/core/log.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace cubos::core;

struct ImGuiData
{
    io::Window& window;
    gl::RenderDevice& renderDevice;

    size_t onMouseMovedId;
    size_t onMouseDownId;
    size_t onMouseUpId;
    size_t onMouseScrollId;
    size_t onKeyDownId;
    size_t onKeyUpId;
    size_t onCharId;

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
    size_t vbSize, ibSize;

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

static int keyToImGuiKey(io::Key key)
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

static void createDeviceObjects()
{
    auto& io = ImGui::GetIO();
    auto bd = (ImGuiData*)io.BackendPlatformUserData;

    // Setup render states.

    gl::RasterStateDesc rasterStateDesc;
    rasterStateDesc.cullEnabled = false;
    rasterStateDesc.rasterMode = gl::RasterMode::Fill;
    rasterStateDesc.scissorEnabled = true;
    bd->rasterState = bd->renderDevice.createRasterState(rasterStateDesc);

    gl::BlendStateDesc blendStateDesc;
    blendStateDesc.blendEnabled = true;
    blendStateDesc.color.src = gl::BlendFactor::SrcAlpha;
    blendStateDesc.color.dst = gl::BlendFactor::InvSrcAlpha;
    blendStateDesc.color.op = gl::BlendOp::Add;
    blendStateDesc.alpha.src = gl::BlendFactor::One;
    blendStateDesc.alpha.dst = gl::BlendFactor::InvSrcAlpha;
    blendStateDesc.alpha.op = gl::BlendOp::Add;
    bd->blendState = bd->renderDevice.createBlendState(blendStateDesc);

    gl::DepthStencilStateDesc depthStencilStateDesc;
    depthStencilStateDesc.depth.enabled = false;
    depthStencilStateDesc.stencil.enabled = false;
    bd->depthStencilState = bd->renderDevice.createDepthStencilState(depthStencilStateDesc);

    // Setup shader pipeline.
    auto vs = bd->renderDevice.createShaderStage(gl::Stage::Vertex, R"glsl(
#version 330 core

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

    auto ps = bd->renderDevice.createShaderStage(gl::Stage::Pixel, R"glsl(
#version 330 core

uniform sampler2D tex;

in vec2 fragUv;
in vec4 fragColor;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = fragColor * texture(tex, fragUv);
}
    )glsl");

    bd->pipeline = bd->renderDevice.createShaderPipeline(vs, ps);
    bd->textureBP = bd->pipeline->getBindingPoint("tex");
    bd->cbBP = bd->pipeline->getBindingPoint("Proj");

    // Build texture atlas.
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
    {
        logCritical("Failed to create ImGui font texture");
        abort();
    }
    io.Fonts->SetTexID((void*)bd->texture.get());

    // Create projection constant buffer.
    bd->cb = bd->renderDevice.createConstantBuffer(sizeof(glm::mat4), nullptr, gl::Usage::Dynamic);

    // Initialize buffer sizes.
    bd->vbSize = 0;
    bd->ibSize = 0;
}

void ui::init(io::Window& window)
{
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    if (io.BackendPlatformUserData != nullptr)
    {
        logWarning("ui::init: already initialized");
        return;
    }

    // Setup back-end capabilities flags
    ImGuiData* bd = IM_NEW(ImGuiData){window, window.getRenderDevice()};
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_cubos";

    // Create mouse cursors.
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    bd->cursors[ImGuiMouseCursor_Arrow] = window.createCursor(io::Cursor::Standard::Arrow);
    bd->cursors[ImGuiMouseCursor_TextInput] = window.createCursor(io::Cursor::Standard::IBeam);
    bd->cursors[ImGuiMouseCursor_ResizeAll] = window.createCursor(io::Cursor::Standard::AllResize);
    bd->cursors[ImGuiMouseCursor_ResizeNS] = window.createCursor(io::Cursor::Standard::NSResize);
    bd->cursors[ImGuiMouseCursor_ResizeEW] = window.createCursor(io::Cursor::Standard::EWResize);
    bd->cursors[ImGuiMouseCursor_ResizeNESW] = window.createCursor(io::Cursor::Standard::NESWResize);
    bd->cursors[ImGuiMouseCursor_ResizeNWSE] = window.createCursor(io::Cursor::Standard::NWSEResize);
    bd->cursors[ImGuiMouseCursor_Hand] = window.createCursor(io::Cursor::Standard::Hand);
    bd->cursors[ImGuiMouseCursor_NotAllowed] = window.createCursor(io::Cursor::Standard::NotAllowed);
    for (int i = 0; i < ImGuiMouseCursor_COUNT; i++)
        if (bd->cursors[i] == nullptr)
            bd->cursors[i] = bd->cursors[ImGuiMouseCursor_Arrow];

    bd->onMouseMovedId = window.onMouseMoved.registerCallback([](glm::ivec2 pos) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(static_cast<float>(pos.x), static_cast<float>(pos.y));
    });

    bd->onMouseDownId = window.onMouseDown.registerCallback([](io::MouseButton button) {
        ImGuiIO& io = ImGui::GetIO();
        int b = buttonToImGuiButton(button);
        if (b != -1)
            io.AddMouseButtonEvent(b, true);
    });

    bd->onMouseUpId = window.onMouseUp.registerCallback([](io::MouseButton button) {
        ImGuiIO& io = ImGui::GetIO();
        int b = buttonToImGuiButton(button);
        if (b != -1)
            io.AddMouseButtonEvent(b, false);
    });

    bd->onMouseScrollId = window.onMouseScroll.registerCallback([](glm::ivec2 scroll) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(static_cast<float>(scroll.x), static_cast<float>(scroll.y));
    });

    bd->onKeyDownId = window.onKeyDown.registerCallback([](io::Key key) {
        ImGuiIO& io = ImGui::GetIO();
        int k = keyToImGuiKey(key);
        if (k != -1)
            io.AddKeyEvent(k, true);
    });

    bd->onKeyUpId = window.onKeyUp.registerCallback([](io::Key key) {
        ImGuiIO& io = ImGui::GetIO();
        int k = keyToImGuiKey(key);
        if (k != -1)
            io.AddKeyEvent(k, false);
    });

    bd->onCharId = window.onChar.registerCallback([](char c) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(c);
    });

    bd->time = window.getTime();

    createDeviceObjects();
}

void ui::terminate()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData != nullptr);
    ImGuiData* bd = (ImGuiData*)io.BackendPlatformUserData;

    for (int i = 0; i < ImGuiMouseCursor_COUNT; ++i)
    {
        bd->cursors[i] = nullptr;
    }

    bd->window.onMouseMoved.unregisterCallback(bd->onMouseMovedId);
    bd->window.onMouseDown.unregisterCallback(bd->onMouseDownId);
    bd->window.onMouseUp.unregisterCallback(bd->onMouseUpId);
    bd->window.onMouseScroll.unregisterCallback(bd->onMouseScrollId);
    bd->window.onKeyDown.unregisterCallback(bd->onKeyDownId);
    bd->window.onKeyUp.unregisterCallback(bd->onKeyUpId);
    bd->window.onChar.unregisterCallback(bd->onCharId);

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

    ImGui::DestroyContext();
}

void ui::beginFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiData* bd = (ImGuiData*)io.BackendPlatformUserData;
    io.DisplaySize.x = bd->window.getFramebufferSize().x;
    io.DisplaySize.y = bd->window.getFramebufferSize().y;
    // TODO: handle framebuffer scale

    // Setup time step.
    double time = bd->window.getTime();
    io.DeltaTime = static_cast<float>(time - bd->time);
    bd->time = time;

    // Update cursor.
    if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) &&
        bd->window.getMouseState() != io::MouseState::Locked)
    {
        ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
        if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor.
            bd->window.setMouseState(io::MouseState::Hidden);
        }
        else
        {
            // Show OS mouse cursor.
            bd->window.setCursor(bd->cursors[imguiCursor]);
            bd->window.setMouseState(io::MouseState::Default);
        }
    }

    ImGui::NewFrame();
}

static void setupRenderState(ImGuiData* bd)
{
    bd->renderDevice.setRasterState(bd->rasterState);
    bd->renderDevice.setBlendState(bd->blendState);
    bd->renderDevice.setDepthStencilState(bd->depthStencilState);
    bd->renderDevice.setShaderPipeline(bd->pipeline);
}

void ui::endFrame(gl::Framebuffer target)
{
    ImGuiData* bd = (ImGuiData*)ImGui::GetIO().BackendPlatformUserData;
    ImGui::Render();
    auto drawData = ImGui::GetDrawData();

    // Upload projection matrix to constant buffer.
    glm::mat4& proj = *(glm::mat4*)bd->cb->map();
    proj = glm::ortho(drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x,
                      drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y);
    bd->cb->unmap();

    // Set render state.
    setupRenderState(bd);
    bd->renderDevice.setViewport(0, 0, drawData->DisplaySize.x, drawData->DisplaySize.y);

    // Render command lists.
    ImVec2 clipOff = drawData->DisplayPos;
    for (int n = 0; n < drawData->CmdListsCount; ++n)
    {
        const auto* cmdList = drawData->CmdLists[n];

        // Create and grow vertex buffer if needed.
        if (!bd->vb || bd->vbSize < cmdList->VtxBuffer.Size)
        {
            bd->vbSize = cmdList->VtxBuffer.Size + 5000;
            bd->vb = bd->renderDevice.createVertexBuffer(bd->vbSize * sizeof(ImDrawVert), nullptr, gl::Usage::Dynamic);

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
            bd->va = bd->renderDevice.createVertexArray(desc);
        }

        // Create and grow index buffer if needed.
        if (!bd->ib || bd->ibSize < cmdList->IdxBuffer.Size)
        {
            bd->ibSize = cmdList->IdxBuffer.Size + 10000;
            bd->ib = bd->renderDevice.createIndexBuffer(
                bd->ibSize * sizeof(ImDrawIdx), nullptr,
                sizeof(ImDrawIdx) == 2 ? gl::IndexFormat::UShort : gl::IndexFormat::UInt, gl::Usage::Dynamic);
        }

        // Upload vertex and index data into vertex buffer.
        ImDrawVert* vtxDst = (ImDrawVert*)bd->vb->map();
        ImDrawIdx* idxDst = (ImDrawIdx*)bd->ib->map();
        memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        bd->vb->unmap();
        bd->ib->unmap();

        bd->renderDevice.setVertexArray(bd->va);
        bd->renderDevice.setIndexBuffer(bd->ib);
        bd->textureBP->bind(bd->texture);
        bd->cbBP->bind(bd->cb);

        for (int i = 0; i < cmdList->CmdBuffer.Size; i++)
        {
            const auto* cmd = &cmdList->CmdBuffer[i];
            if (cmd->UserCallback != nullptr)
            {
                if (cmd->UserCallback == ImDrawCallback_ResetRenderState)
                {
                    setupRenderState(bd);
                    bd->renderDevice.setVertexArray(bd->va);
                    bd->renderDevice.setIndexBuffer(bd->ib);
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
                bd->renderDevice.setScissor(clipMin.x, drawData->DisplaySize.y - clipMax.y, clipMax.x - clipMin.x,
                                            clipMax.y - clipMin.y);

                // Bind the texture and draw.
                bd->renderDevice.drawTrianglesIndexed(cmd->IdxOffset, cmd->ElemCount);
            }
        }
    }
}
