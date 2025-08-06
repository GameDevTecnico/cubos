#include "layout.hpp"

#include <imgui_internal.h>

#include <utility>

using tesseratos::Layout;
using tesseratos::LayoutNode;

LayoutNode LayoutNode::loadFromJson(nlohmann::json json)
{
    LayoutNode node;

    if (!json.contains("content"))
    {
        CUBOS_ERROR("LayoutNode::loadFromJson: invalid layout object:\n{}", json.dump(4));
        return node;
    }

    auto content = json["content"];

    for (const auto& el : content)
    {

        if (el.contains("split"))
        {
            if (!(el["split"].contains("ratio") && el["split"].contains("direction")))
            {
                CUBOS_ERROR("LayoutNode::loadFromJson: invalid split object:\n{}", el["split"].dump(4));
                return node;
            }

            Split split;
            split.ratio = el["split"]["ratio"];
            split.direction = LayoutNode::direction(el["split"]["direction"]);
            split.content = std::make_unique<LayoutNode>(LayoutNode::loadFromJson(el));

            node.mWindowSplits.emplace_back(std::move(split));
        }
        else
        {
            if (el.is_object())
            {
                node.mContent = std::make_unique<LayoutNode>(LayoutNode::loadFromJson(el));
            }
            else if (el.is_string())
            {
                node.mWindows.push_back(el);
            }
            else
            {
                CUBOS_ERROR("LayoutNode::loadFromJson: invalid content object:\n{}", el.dump(4));
                return node;
            }
        }
    }

    return node;
}

void LayoutNode::applyToImGui(const ImGuiID root)
{
    ImGuiID newRoot = root;
    for (const auto& split : this->mWindowSplits)
    {
        ImGuiID splitRoot = ImGui::DockBuilderSplitNode(root, split.direction, split.ratio, nullptr, &newRoot);
        split.content->applyToImGui(splitRoot);
    }

    if (this->mContent)
    {
        this->mContent->applyToImGui(newRoot);
    }

    for (const auto& window : this->mWindows)
    {
        ImGui::DockBuilderDockWindow(window.c_str(), newRoot);
    }
}

Layout Layout::loadFromJson(nlohmann::json json)
{
    Layout newLayout;

    newLayout.mContent = std::make_unique<LayoutNode>(LayoutNode::loadFromJson(std::move(json)));

    return newLayout;
}

void Layout::applyToImGui(const ImGuiID root)
{
    this->mContent->applyToImGui(root);
}

ImGuiDir LayoutNode::direction(const std::string dir)
{
    if (dir == "up") { return ImGuiDir_Up; }
    if (dir == "down") { return ImGuiDir_Down; }
    if (dir == "left") { return ImGuiDir_Left; }
    if (dir == "right") { return ImGuiDir_Right; }
    return ImGuiDir_None;
}
