#include <algorithm>
#include <numeric> //Included for the accumulated/max_element funcs

#include <imgui.h>
#include <implot.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/metrics_panel/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

namespace
{
    struct Metrics
    {
        CUBOS_ANONYMOUS_REFLECT(Metrics);

        float timeElapsed = 0.0F;
        float fps = 0.0F;
        float maxFps = 0.0F;
        float avgFps;
        // FPS Graph related
        std::vector<float> timeHistory;
        std::vector<float> fpsHistory;
        const float timeSample = -5.0F;
        const float fpsSample = 1000.0F;
    };
} // namespace

void cubos::engine::metricsPanelPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(toolboxPlugin);

    cubos.resource<Metrics>();

    cubos.system("show Metrics UI")
        .tagged(imguiTag)
        .call([](Toolbox& toolbox, const DeltaTime& deltaTime, Metrics& metrics) {
            if (!toolbox.isOpen("Metrics Panel"))
            {
                return;
            }

            ImGui::Begin("Metrics Panel");
            // General Metrics
            metrics.timeElapsed += deltaTime.value();
            metrics.fps = 1 / deltaTime.value();

            for (std::size_t i = 0; i < metrics.timeHistory.size(); ++i)
            {
                metrics.timeHistory[i] -= deltaTime.value();
                if (metrics.timeHistory[i] <= metrics.timeSample)
                {
                    metrics.timeHistory.erase(metrics.timeHistory.begin() +
                                              static_cast<std::vector<float>::difference_type>(i));
                    metrics.fpsHistory.erase(metrics.fpsHistory.begin() +
                                             static_cast<std::vector<float>::difference_type>(i));
                }
            }

            // Add the current values to the history
            metrics.timeHistory.push_back(0);
            metrics.fpsHistory.push_back(metrics.fps);

            ImGui::Text("Time: %.2f", metrics.timeElapsed);
            // Plot the FPS history
            if (ImPlot::BeginPlot("FPS Graph:"))
            {
                ImGui::Text("FPS: %.2f", metrics.fps);
                ImPlot::SetupAxes("Time", "FPS",
                                  ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations,
                                  ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations);
                ImPlot::SetupAxesLimits(metrics.timeSample, 0.0F, 0.0F, metrics.fpsSample, ImPlotCond_Always);
                ImPlot::SetNextFillStyle(ImVec4(0, 0, 0, -1), 0.0F);
                ImPlot::PlotLine("FPS Graph", metrics.timeHistory.data(), metrics.fpsHistory.data(),
                                 static_cast<int>(metrics.timeHistory.size()),
                                 ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs);
                ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "Max: %.2f",
                                   *std::max_element(metrics.fpsHistory.begin(), metrics.fpsHistory.end()));
                ImGui::TextColored(ImVec4(1.0F, 1.0F, 0.0F, 1.0F), "Avg: %.2f",
                                   (std::accumulate(metrics.fpsHistory.begin(), metrics.fpsHistory.end(), 0.0F) /
                                    static_cast<float>(metrics.fpsHistory.size())));
                ImPlot::EndPlot();
            }
            ImGui::End();
        });
}
