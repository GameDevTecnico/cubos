/// @file
/// @brief Class @ref tesseratos::Layout.
/// @ingroup layout-plugin

#pragma once

#include <unordered_map>

#include <nlohmann/json.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/prelude.hpp>

namespace tesseratos
{

    /// @brief Describes a layout node for tesseratos.
    class LayoutNode
    {
    public:
        static LayoutNode createFromImGui();
        static LayoutNode loadFromJson(nlohmann::json json);

        void applyToImGui(const ImGuiID root);

    private:
        struct Split
        {
            float ratio;
            ImGuiDir direction;
            std::unique_ptr<LayoutNode> content;
        };

        std::vector<Split> windowSplits;
        std::unique_ptr<LayoutNode> content;
        std::vector<std::string> windows;

        static ImGuiDir direction(std::string dir);
    };

    /// @brief Describes a layout for tesseratos.
    class Layout
    {
    public:
        static LayoutNode createFromImGui();
        static Layout loadFromJson(nlohmann::json json);

        void applyToImGui(const ImGuiID root);

    private:
        std::unique_ptr<LayoutNode> content;
    };
} // namespace tesseratos
