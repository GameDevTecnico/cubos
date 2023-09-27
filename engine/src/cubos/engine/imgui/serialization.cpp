#include <array>

#include <imgui.h>

#include <cubos/engine/imgui/serialization.hpp>

using namespace cubos::engine;

using Package = cubos::core::data::Package;

// Converts a scalar (must not be an Object, Array or Dictionary) value to a
// string.
static std::string valueToString(const Package& pkg)
{
    switch (pkg.type())
    {
    case Package::Type::None:
        return "None";
    case Package::Type::I8:
        return std::to_string(pkg.get<int8_t>());
    case Package::Type::I16:
        return std::to_string(pkg.get<int16_t>());
    case Package::Type::I32:
        return std::to_string(pkg.get<int32_t>());
    case Package::Type::I64:
        return std::to_string(pkg.get<int64_t>());
    case Package::Type::U8:
        return std::to_string(pkg.get<uint8_t>());
    case Package::Type::U16:
        return std::to_string(pkg.get<uint16_t>());
    case Package::Type::U32:
        return std::to_string(pkg.get<uint32_t>());
    case Package::Type::U64:
        return std::to_string(pkg.get<uint64_t>());
    case Package::Type::F32:
        return std::to_string(pkg.get<float>());
    case Package::Type::F64:
        return std::to_string(pkg.get<double>());
    case Package::Type::Bool:
        return pkg.get<bool>() ? "true" : "false";
    case Package::Type::String:
        return "\"" + pkg.get<std::string>() + "\"";
    default:
        CUBOS_CRITICAL("Expected scalar value, got object/array/dictionary");
        abort();
    }
}

static void showInternal(const Package& pkg, const std::string& name);

static void showScalar(const Package& pkg, const std::string& name)
{
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::TreeNodeEx(name.c_str(),
                      ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
    ImGui::TableSetColumnIndex(1);

    switch (pkg.type())
    {
    case Package::Type::None:
        ImGui::Text("None");
        break;
    case Package::Type::I8:
        ImGui::Text("%d", pkg.get<int8_t>());
        break;
    case Package::Type::I16:
        ImGui::Text("%d", pkg.get<int16_t>());
        break;
    case Package::Type::I32:
        ImGui::Text("%d", pkg.get<int32_t>());
        break;
    case Package::Type::I64:
        ImGui::Text("%lld", static_cast<long long>(pkg.get<int64_t>()));
        break;
    case Package::Type::U8:
        ImGui::Text("%u", pkg.get<uint8_t>());
        break;
    case Package::Type::U16:
        ImGui::Text("%u", pkg.get<uint16_t>());
        break;
    case Package::Type::U32:
        ImGui::Text("%u", pkg.get<uint32_t>());
        break;
    case Package::Type::U64:
        ImGui::Text("%llu", static_cast<unsigned long long>(pkg.get<uint64_t>()));
        break;
    case Package::Type::F32:
        ImGui::Text("%f", pkg.get<float>());
        break;
    case Package::Type::F64:
        ImGui::Text("%f", pkg.get<double>());
        break;
    case Package::Type::Bool:
        ImGui::Text("%s", pkg.get<bool>() ? "true" : "false");
        break;
    case Package::Type::String:
        ImGui::Text("\"%s\"", pkg.get<std::string>().c_str());
        break;
    default:
        abort();
    }
}

static void showStructured(const Package& pkg, const std::string& name)
{
    const char* elements;
    switch (pkg.type())
    {
    case Package::Type::Object:
        elements = "fields";
        break;
    case Package::Type::Array:
        elements = "elements";
        break;
    case Package::Type::Dictionary:
        elements = "entries";
        break;
    default:
        abort();
    }

    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool nodeOpen = ImGui::TreeNode(name.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%d %s", static_cast<int>(pkg.size()), elements);
    if (nodeOpen)
    {
        for (std::size_t i = 0; i < pkg.size(); ++i)
        {
            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(i));
            switch (pkg.type())
            {
            case Package::Type::Object:
                showInternal(pkg.fields()[i].second, pkg.fields()[i].first);
                break;
            case Package::Type::Array:
                showInternal(pkg.elements()[i], std::to_string(i));
                break;
            case Package::Type::Dictionary:
                showInternal(pkg.dictionary()[i].second, valueToString(pkg.dictionary()[i].first));
                break;
            default:
                abort();
            }
            ImGui::PopID();
        }

        ImGui::TreePop();
    }
}

static void showInternal(const Package& pkg, const std::string& name)
{
    pkg.isStructured() ? showStructured(pkg, name) : showScalar(pkg, name);
}

void cubos::engine::imguiShowPackage(const Package& pkg, const std::string& name)
{
    ImGui::TableNextRow();
    showInternal(pkg, name);
}

static bool editInternal(Package& pkg, const std::string& name);

static bool pickScalar(Package& pkg, const std::string& name)
{
    switch (pkg.type())
    {
    case Package::Type::None:
        ImGui::Text("None");
        return true;
    case Package::Type::I8: {
        auto value = pkg.get<int8_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_S8, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::I16: {
        auto value = pkg.get<int16_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_S16, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::I32: {
        auto value = pkg.get<int32_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_S32, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::I64: {
        auto value = pkg.get<int64_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_S64, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::U8: {
        auto value = pkg.get<uint8_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_U8, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::U16: {
        auto value = pkg.get<uint16_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_U16, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::U32: {
        auto value = pkg.get<uint32_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::U64: {
        auto value = pkg.get<uint64_t>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_U64, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::F32: {
        auto value = pkg.get<float>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_Float, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::F64: {
        auto value = pkg.get<double>();
        if (ImGui::InputScalar(name.c_str(), ImGuiDataType_Double, &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::Bool: {
        auto value = pkg.get<bool>();
        if (ImGui::Checkbox(name.c_str(), &value))
        {
            pkg.set(value);
            return true;
        }
        break;
    }
    case Package::Type::String: {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        strncpy(buf, pkg.get<std::string>().c_str(), sizeof(buf) - 1);
        if (ImGui::InputText(name.c_str(), buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            pkg.set(std::string(buf));
            return true;
        }
        break;
    }
    default:
        abort();
    }

    return false;
}

static bool editScalar(Package& pkg, const std::string& name)
{
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::TreeNodeEx(name.c_str(),
                      ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
    ImGui::PushID(name.c_str());
    ImGui::TableSetColumnIndex(1);
    bool ret = pickScalar(pkg, name);
    ImGui::PopID();
    return ret;
}

static bool editStructured(Package& pkg, const std::string& name)
{
    bool changed = false;

    const char* elements;
    switch (pkg.type())
    {
    case Package::Type::Object:
        elements = "fields";
        break;
    case Package::Type::Array:
        elements = "elements";
        break;
    case Package::Type::Dictionary:
        elements = "entries";
        break;
    default:
        abort();
    }

    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool nodeOpen = ImGui::TreeNode(name.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%d %s", static_cast<int>(pkg.size()), elements);

    if (nodeOpen)
    {
        for (std::size_t i = 0; i < pkg.size(); ++i)
        {
            bool shouldErase;
            bool shouldDuplicate;

            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(i));

            switch (pkg.type())
            {
            case Package::Type::Object:
                changed |= editInternal(pkg.fields()[i].second, pkg.fields()[i].first);
                break;
            case Package::Type::Array:
                ImGui::TableSetColumnIndex(2);
                shouldErase = ImGui::Button("-");
                ImGui::SameLine();
                shouldDuplicate = ImGui::Button("x2");
                changed |= editInternal(pkg.elements()[i], std::to_string(i));
                if (shouldErase)
                {
                    pkg.elements().erase(pkg.elements().begin() + static_cast<int>(i));
                    i -= 1;
                    changed = true;
                }
                else if (shouldDuplicate)
                {
                    pkg.elements().insert(pkg.elements().begin() + static_cast<int>(i), pkg.elements()[i]);
                    changed = true;
                }
                break;
            case Package::Type::Dictionary:
                ImGui::TableSetColumnIndex(2);
                shouldErase = ImGui::Button("-");
                ImGui::SameLine();
                shouldDuplicate = ImGui::Button("x2");
                changed |= editInternal(pkg.dictionary()[i].second, valueToString(pkg.dictionary()[i].first));
                if (shouldErase)
                {
                    pkg.dictionary().erase(pkg.dictionary().begin() + static_cast<int>(i));
                    i -= 1;
                    changed = true;
                }
                else if (shouldDuplicate)
                {
                    ImGui::OpenPopup("Duplicate");
                }

                if (ImGui::BeginPopup("Duplicate"))
                {
                    auto key = pkg.dictionary()[i].first;
                    pickScalar(key, "Key");
                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        pkg.dictionary().emplace(pkg.dictionary().begin(),
                                                 std::make_pair(key, pkg.dictionary()[i].second));
                        i += 1;
                        changed = true;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                break;
            default:
                abort();
            }

            ImGui::PopID();
        }

        ImGui::TreePop();
    }

    return changed;
}

static bool editInternal(Package& pkg, const std::string& name)
{
    return pkg.isStructured() ? editStructured(pkg, name) : editScalar(pkg, name);
}

bool cubos::engine::imguiEditPackage(Package& pkg, const std::string& name)
{
    ImGui::TableNextRow();
    return editInternal(pkg, name);
}
