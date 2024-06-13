#include "plugin.hpp"

#include <imgui.h>

#include <cubos/core/io/window.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

#include "../game/plugin.hpp"
#include "window.hpp"

using namespace cubos::core::io;
using namespace cubos::core::gl;
using namespace cubos::engine;

namespace
{
    struct State
    {
        std::shared_ptr<tesseratos::GameWindow> window{nullptr};
        Texture2D output{nullptr};
    };
} // namespace

static void fakeWindowPlugin(Cubos& cubos)
{
    cubos.uninitResource<Window>(); // Inserted externally by the gameWindowPlugin.
    cubos.event<WindowEvent>();

    cubos.startupTag(windowInitTag);
    cubos.tag(windowPollTag);
    cubos.tag(windowRenderTag).after(windowPollTag);

    cubos.startupSystem("initialize fake Window").tagged(windowInitTag).call([](Window& window) {
        std::static_pointer_cast<tesseratos::GameWindow>(window)->reset();
    });

    cubos.system("poll fake Window events")
        .tagged(windowPollTag)
        .call([](Window& window, EventWriter<WindowEvent> events) {
            while (auto event = window->pollEvent())
            {
                events.push(event.value());
            }
        });

    cubos.system("swap fake Window buffers").tagged(windowRenderTag).call([](Window& window) {
        window->swapBuffers();
    });
}

void tesseratos::gameWindowPlugin(Cubos& cubos)
{
    cubos.depends(windowPlugin);
    cubos.depends(imguiPlugin);
    cubos.depends(gamePlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("inject Game Window plugin")
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, Game& game) {
            auto fakeWindow = std::make_shared<GameWindow>(window);
            game.inject(windowPlugin, fakeWindowPlugin);
            game.resource<Window>(fakeWindow);
            cmds.emplaceResource<State>(State{.window = fakeWindow});
        });

    cubos.system("show Game Window")
        .tagged(imguiTag)
        .call([](const Game& game, State& state, EventReader<WindowEvent> events) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

            ImGui::SetNextWindowSizeConstraints(ImVec2(400, 300), ImVec2(FLT_MAX, FLT_MAX));
            if (ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
            {
                if (game.isStarted())
                {
                    // Update the window coordinates.
                    state.window->setMinCoords(glm::uvec2(static_cast<uint32_t>(ImGui::GetCursorScreenPos().x),
                                                          static_cast<uint32_t>(ImGui::GetCursorScreenPos().y)));

                    // Pass events to the window if it's focused.
                    if (state.window->focused())
                    {
                        for (auto event : events)
                        {
                            state.window->handleEvent(std::move(event));
                        }

                        // Allow the user to escape the window focus.
                        if (state.window->pressed(Key::Escape, Modifiers::Alt))
                        {
                            state.window->setFocused(false);
                        }

                        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
                    }
                    else
                    {
                        while (events.read() != nullptr)
                        {
                            // Discard all events if the window is not focused.
                        }

                        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
                    }

                    // Get window size and show the output.
                    glm::uvec2 size = {static_cast<uint32_t>(ImGui::GetWindowWidth()),
                                       static_cast<uint32_t>(ImGui::GetWindowHeight())};
                    size.y -= static_cast<uint32_t>(ImGui::GetCursorPosY()); // Make space for the title bar.

                    state.output = state.window->output();
                    if (state.output != nullptr)
                    {
                        // We store the output texture's handle to make sure it is not deleted until the next frame.
                        // This is necessary as we're passing its raw pointer to ImGui, which won't increase the
                        // reference count.
                        //
                        // We also flip the texture UVs vertically to match ImGui's coordinate system.
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        ImGui::ImageButton("output", state.output.get(),
                                           ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)), ImVec2(0, 1),
                                           ImVec2(1, 0));
                        ImGui::PopStyleVar();

                        if (state.window->mouseState() == MouseState::Default)
                        {
                            state.window->setFocused(ImGui::IsItemHovered());
                        }
                        else if (ImGui::IsItemClicked())
                        {
                            state.window->setFocused(true);
                        }
                    }

                    // Resize window if needed.
                    if (size != state.window->size())
                    {
                        state.window->resize(size);
                    }
                }
                else
                {
                    ImGui::Text("Not running");
                }
            }

            ImGui::End();
            ImGui::PopStyleVar();
        });
}
