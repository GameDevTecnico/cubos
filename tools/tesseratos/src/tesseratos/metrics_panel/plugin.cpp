#include <numeric> //Included for the accumulated/max_element funcs

#include <imgui.h>
#include <implot.h>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/metrics_panel/plugin.hpp>

using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::engine::Cubos;
using cubos::engine::DeltaTime;

using namespace tesseratos;

namespace
{
    struct Metrics
    {
        float timeElapsed = 0.0F;
        float fps = 0.0F;
        float maxFps = 0.0F;
        float avgFps;
        // FPSGraph related
        std::vector<float> timeHistory;
        std::vector<float> fpsHistory;
        const float timeSample = -5.0F;
        const float fpsSample = 1000.0F;
    };
} // namespace

static void showMetricsSystem(Read<DeltaTime> deltaTime, Write<Metrics> metrics)
{
    ImGui::Begin("Metrics Panel");
    // General Metrics
    metrics->timeElapsed += deltaTime->value;
    metrics->fps = 1 / deltaTime->value;

    for (unsigned int i = 0; i < metrics->timeHistory.size(); i++)
    {
        metrics->timeHistory[i] -= deltaTime->value;
        if (metrics->timeHistory[i] <= metrics->timeSample)
        {
            metrics->timeHistory.erase(metrics->timeHistory.begin() + i);
            metrics->fpsHistory.erase(metrics->fpsHistory.begin() + i);
        }
    }

    // Add the current values to the history
    metrics->timeHistory.push_back(0);
    metrics->fpsHistory.push_back(metrics->fps);

    ImGui::Text("Time: %.2f", metrics->timeElapsed);
    // Plot the FPS history
    if (ImPlot::BeginPlot("FPS Graph:"))
    {
        ImGui::Text("FPS: %.2f", metrics->fps);
        ImPlot::SetupAxes("Time", "FPS",
                          ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations,
                          ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(metrics->timeSample, 0.0F, 0.0F, metrics->fpsSample, ImPlotCond_Always);
        ImPlot::SetNextFillStyle(ImVec4(0, 0, 0, -1), 0.0F);
        ImPlot::PlotLine("FPS Graph", metrics->timeHistory.data(), metrics->fpsHistory.data(),
                         static_cast<int>(metrics->timeHistory.size()), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs);
        // This dont work return inf and -inf respectivl; Actuallly it might work
        ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "Max: %.2f",
                           *std::max_element(metrics->fpsHistory.begin(), metrics->fpsHistory.end()));
        ImGui::TextColored(ImVec4(1.0F, 1.0F, 0.0F, 1.0F), "Avg: %.2f",
                           (std::accumulate(metrics->fpsHistory.begin(), metrics->fpsHistory.end(), 0.0F) /
                            static_cast<float>(metrics->fpsHistory.size())));
        ImPlot::EndPlot();
    }
    ImGui::End();
}

void tesseratos::metricsPanelPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addResource<Metrics>();
    cubos.system(showMetricsSystem).tagged("cubos.imgui");
}
