#include <imgui.h>

#include <cubos/core/gl/material.hpp>
#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/ui/imgui.hpp>
#include <cubos/core/ui/serialization.hpp>

using namespace cubos::core;

int main()
{
    initializeLogger();
    auto window = io::openWindow();
    auto& renderDevice = window->getRenderDevice();
    ui::initialize(window);

    std::vector<std::unordered_map<std::string, gl::Material>> matDictArray = {
        {{"mat1", gl::Material()}, {"mat2", gl::Material()}},
        {{"mat3", gl::Material()}, {"mat4", gl::Material()}, {"mat5", gl::Material()}},
        {{"mat6", gl::Material()}}};
    int8_t i8 = INT8_MIN;
    int16_t i16 = INT16_MIN;
    int32_t i32 = INT32_MIN;
    int64_t i64 = INT64_MIN;
    uint8_t u8 = UINT8_MAX;
    uint16_t u16 = UINT16_MAX;
    uint32_t u32 = UINT32_MAX;
    uint64_t u64 = UINT64_MAX;
    float f32 = -FLT_MAX;
    double f64 = -DBL_MAX;
    bool boolean = false;
    std::string string = "poggers";

    while (!window->shouldClose())
    {
        ui::beginFrame();

        ImGui::Begin("Inspector");
        ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
        ui::show(i8, "i8");
        ui::show(i16, "i16");
        ui::show(i32, "i32");
        ui::show(i64, "i64");
        ui::show(u8, "u8");
        ui::show(u16, "u16");
        ui::show(u32, "u32");
        ui::show(u64, "u64");
        ui::show(f32, "f32");
        ui::show(f64, "f64");
        ui::show(boolean, "boolean");
        ui::show(string, "string");
        ui::show(matDictArray, "Mat Dict Array");
        ImGui::EndTable();
        ImGui::End();

        ImGui::Begin("Editor");
        ImGui::BeginTable("split", 3, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable);
        ui::edit(i8, "i8");
        ui::edit(i16, "i16");
        ui::edit(i32, "i32");
        ui::edit(i64, "i64");
        ui::edit(u8, "u8");
        ui::edit(u16, "u16");
        ui::edit(u32, "u32");
        ui::edit(u64, "u64");
        ui::edit(f32, "f32");
        ui::edit(f64, "f64");
        ui::edit(boolean, "boolean");
        ui::edit(string, "string");
        ui::edit(matDictArray, "Mat Dict Array");
        ImGui::EndTable();
        ImGui::End();

        ImGui::ShowDemoWindow();

        auto sz = window->getFramebufferSize();
        renderDevice.setRasterState(nullptr);
        renderDevice.setBlendState(nullptr);
        renderDevice.setDepthStencilState(nullptr);
        renderDevice.setViewport(0, 0, sz.x, sz.y);
        renderDevice.clearColor(0.0F, 0.0F, 0.0F, 1.0F);

        ui::endFrame();
        window->swapBuffers();
        while (auto event = window->pollEvent())
        {
            ui::handleEvent(event.value());
        }
    }

    ui::terminate();
    return 0;
}
