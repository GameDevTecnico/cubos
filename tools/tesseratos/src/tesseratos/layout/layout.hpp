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
    ///
    /// A layout defines how the dockspace is split and wich windows are docked where.
    class Layout
    {
    public:
        CUBOS_REFLECT;

        /// @brief Empty constructor.
        Layout() = default;

        /// @brief Loads the scene node from a JSON object.
        /// @param json JSON object.
        /// @param components Component type registry.
        /// @param relations Relation type registry.
        /// @return Whether the scene node was successfully loaded.
        void load(const ImGuiID root, const nlohmann::json& json);

        static Layout loadFromJson(nlohmann::json json);

        void applyToImGui(const ImGuiID root);

    private:
        std::unique_ptr<LayoutNode> content;

        ImGuiDir direction(std::string dir);
    };
} // namespace tesseratos
