#include <cubos/core/ui/serialization.hpp>

#include <imgui.h>

using namespace cubos::core;
using namespace cubos::core::ui;

using Type = data::Package::Type;

// Converts a scalar (must not be an Object, Array or Dictionary) value to a
// string.
static std::string valueToString(const data::Package& pkg)
{
    switch (pkg.type())
    {
    case Type::None:
        return "None";
    case Type::I8:
        return std::to_string(pkg.get<int8_t>());
    case Type::I16:
        return std::to_string(pkg.get<int16_t>());
    case Type::I32:
        return std::to_string(pkg.get<int32_t>());
    case Type::I64:
        return std::to_string(pkg.get<int64_t>());
    case Type::U8:
        return std::to_string(pkg.get<uint8_t>());
    case Type::U16:
        return std::to_string(pkg.get<uint16_t>());
    case Type::U32:
        return std::to_string(pkg.get<uint32_t>());
    case Type::U64:
        return std::to_string(pkg.get<uint64_t>());
    case Type::F32:
        return std::to_string(pkg.get<float>());
    case Type::F64:
        return std::to_string(pkg.get<double>());
    case Type::Bool:
        return pkg.get<bool>() ? "true" : "false";
    case Type::String:
        return "\"" + pkg.get<std::string>() + "\"";
    default:
        logCritical("ui::showPackage() failed: expected scalar value, got object/array/dictionary");
        abort();
    }
}

void cubos::core::ui::showPackage(const data::Package& pkg, const std::string& name)
{
    ImGui::TableNextRow();
    if (pkg.type() == Type::Object)
    {
        // The field count must be shown first otherwise it is moved below the
        // expanded fields.
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d fields", static_cast<int>(pkg.size()));
        ImGui::TableSetColumnIndex(0);
        bool nodeOpen = ImGui::TreeNode(name.c_str());
        if (nodeOpen)
        {
            for (const auto& [key, value] : pkg.fields())
            {
                ImGui::TableNextRow();
                showPackage(value, key);
            }
            ImGui::TreePop();
        }
    }
    else if (pkg.type() == Type::Array)
    {
        // The array size must be shown first otherwise it is moved below the
        // expanded array elements.
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d elements", static_cast<int>(pkg.size()));
        ImGui::TableSetColumnIndex(0);
        bool nodeOpen = ImGui::TreeNode(name.c_str());
        if (nodeOpen)
        {
            for (size_t i = 0; i < pkg.size(); ++i)
            {
                ImGui::TableNextRow();
                showPackage(pkg.elements()[i], std::to_string(i));
            }
            ImGui::TreePop();
        }
    }
    else if (pkg.type() == Type::Dictionary)
    {
        // The dictionary size must be shown first otherwise it is moved below
        // the expanded entries.
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d entries", static_cast<int>(pkg.size()));
        ImGui::TableSetColumnIndex(0);
        bool nodeOpen = ImGui::TreeNode(name.c_str());
        if (nodeOpen)
        {
            for (auto& entry : pkg.dictionary())
            {
                ImGui::TableNextRow();
                showPackage(entry.second, valueToString(entry.first));
            }
            ImGui::TreePop();
        }
        ImGui::SameLine();
    }
    else
    {
        ImGui::TableSetColumnIndex(0);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
        ImGui::Text("%s", name.c_str());
        ImGui::TableSetColumnIndex(1);

        switch (pkg.type())
        {
        case Type::None:
            ImGui::Text("None");
            break;
        case Type::I8:
            ImGui::Text("%d", pkg.get<int8_t>());
            break;
        case Type::I16:
            ImGui::Text("%d", pkg.get<int16_t>());
            break;
        case Type::I32:
            ImGui::Text("%d", pkg.get<int32_t>());
            break;
        case Type::I64:
            ImGui::Text("%ld", pkg.get<int64_t>());
            break;
        case Type::U8:
            ImGui::Text("%u", pkg.get<uint8_t>());
            break;
        case Type::U16:
            ImGui::Text("%u", pkg.get<uint16_t>());
            break;
        case Type::U32:
            ImGui::Text("%u", pkg.get<uint32_t>());
            break;
        case Type::U64:
            ImGui::Text("%lu", pkg.get<uint64_t>());
            break;
        case Type::F32:
            ImGui::Text("%f", pkg.get<float>());
            break;
        case Type::F64:
            ImGui::Text("%f", pkg.get<double>());
            break;
        case Type::Bool:
            ImGui::Text("%s", pkg.get<bool>() ? "true" : "false");
            break;
        case Type::String:
            ImGui::Text("\"%s\"", pkg.get<std::string>().c_str());
            break;
        }
    }
}

bool cubos::core::ui::editPackage(data::Package& pkg, const std::string& name)
{
    // TODO.
    return false;
}
