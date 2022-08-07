#include <cubos/core/log.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/gl/material.hpp>
#include <cubos/core/ui/imgui.hpp>
#include <cubos/core/ui/serialization.hpp>

#include <imgui.h>

using namespace cubos::core;

int main(void)
{
    initializeLogger();
    auto window = io::Window::create();
    auto& renderDevice = window->getRenderDevice();
    ui::initialize(*window);

    while (!window->shouldClose())
    {
        ui::beginFrame();

        ImGui::Begin("Inspector");
        ImGui::BeginTable("split", 2);
        ui::show<int8_t>(INT8_MIN, "i8");
        ui::show<int16_t>(INT16_MIN, "i16");
        ui::show<int32_t>(INT32_MIN, "i32");
        ui::show<int64_t>(INT64_MIN, "i64");
        ui::show<uint8_t>(UINT8_MAX, "u8");
        ui::show<uint16_t>(UINT16_MAX, "u16");
        ui::show<uint32_t>(UINT32_MAX, "u32");
        ui::show<uint64_t>(UINT64_MAX, "u64");
        ui::show<float>(2.4323f, "f32");
        ui::show<double>(1.2993, "f32");
        ui::show<bool>(true, "bool");
        ui::show<std::string>("poggers", "bool");
        ui::show(std::unordered_map<uint8_t, gl::Material>({
                     {2, gl::Material()},
                     {3, gl::Material()},
                     {7, gl::Material()},
                 }),
                 "Mat Dict");
        ui::show(std::vector<gl::Material>({gl::Material(), gl::Material()}), "Mat Array");
        ui::show(std::vector<std::unordered_map<std::string, gl::Material>>(
                     {{{"mat1", gl::Material()}, {"mat2", gl::Material()}},
                      {{"mat3", gl::Material()}, {"mat4", gl::Material()}, {"mat5", gl::Material()}},
                      {{"mat6", gl::Material()}}}),
                 "Mat Dict Array");
        ImGui::EndTable();
        ImGui::End();

        ImGui::ShowDemoWindow();

        auto sz = window->getFramebufferSize();
        renderDevice.setRasterState(nullptr);
        renderDevice.setBlendState(nullptr);
        renderDevice.setDepthStencilState(nullptr);
        renderDevice.setViewport(0, 0, sz.x, sz.y);
        renderDevice.clearColor(0.0f, 0.0f, 0.0f, 1.0f);

        ui::endFrame();
        window->swapBuffers();
        window->pollEvents();
    }

    ui::terminate();
    delete window;
    return 0;
}
