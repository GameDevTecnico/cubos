
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>

#include <cubos/engine/imgui/data_inspector.hpp>

using cubos::core::memory::AnyValue;
using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;

using namespace cubos::engine;

void DataInspector::show(const std::string& name, const core::reflection::Type& type, const void* value)
{
    this->inspect(name, type, const_cast<void*>(value), true);
}

bool DataInspector::edit(const std::string& name, const core::reflection::Type& type, void* value)
{
    return this->inspect(name, type, value, false);
}

bool DataInspector::inspect(const std::string& name, const Type& type, void* value, bool readOnly)
{
    ImGui::TableNextRow();
    bool changed = false;
    if (type.has<ArrayTrait>())
    {
        withStructured(name, type, [&]() {
            const auto& trait = type.get<ArrayTrait>();
            auto view = trait.view(value);

            if (readOnly)
            {
                for (auto idx = 0; const auto& element : view)
                {
                    inspect(std::to_string(idx), trait.elementType(), element, readOnly);
                    idx++;
                }
            }
            else
            {
                std::vector<size_t> shouldErase;
                ImGui::PushID("array_elements");
                for (size_t idx = 0; auto* element : view)
                {
                    auto idxStrRepresentation = std::string("[" + std::to_string(idx) + "]");
                    changed |= inspect(idxStrRepresentation, trait.elementType(), element, readOnly);

                    // Removing elements
                    ImGui::SameLine();

                    ImGui::PushID(static_cast<int>(idx));
                    if (ImGui::Button("-"))
                    {
                        shouldErase.push_back(idx);
                    }
                    ImGui::PopID();

                    idx++;
                }
                ImGui::PopID();

                for (const auto& idx : shouldErase)
                {
                    view.erase(idx);
                }

                shouldErase.clear();

                // Adding elements
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::PushID("add_array_element");
                if (ImGui::Button("Add item"))
                {
                    view.insertDefault(view.length());
                    changed = true;
                }
                ImGui::PopID();
            }
        });
    }
    else if (type.has<DictionaryTrait>())
    {
        withStructured(name, type, [&]() {
            const auto& trait = type.get<DictionaryTrait>();
            auto view = trait.view(value);

            if (readOnly)
            {
                for (auto [k, v] : view)
                {
                    auto keyStr = stringKnown(k, trait.keyType());
                    changed |= inspect(keyStr, trait.valueType(), v, readOnly);
                }
            }

            else
            {
                DictionaryTrait::View::Iterator shouldErase = view.end();
                ImGui::PushID("dict_elements");
                for (auto idx = 0; auto [k, v] : view)
                {
                    auto keyStr = stringKnown(k, trait.keyType());
                    changed |= inspect(keyStr, trait.valueType(), v, readOnly);

                    // Removing elements
                    ImGui::SameLine();

                    ImGui::PushID(static_cast<int>(idx));
                    if (ImGui::Button("-"))
                    {
                        shouldErase = view.find(k);
                    }
                    ImGui::PopID();

                    idx++;
                }
                ImGui::PopID();

                if (shouldErase != view.end())
                {
                    view.erase(shouldErase);
                }

                // Adding key
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::PushID("edit_dict_item");

                if (ImGui::Button("Add item"))
                {
                    mKeyExists = false;
                    mKey = AnyValue::defaultConstruct(trait.keyType());
                    mValue = AnyValue::defaultConstruct(trait.valueType());
                    ImGui::OpenPopup("DictPopup");
                }

                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

                if (ImGui::BeginPopupModal("DictPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::BeginTable("DictPopupInsert", 2);
                    inspect("key", trait.keyType(), mKey.get(), readOnly);
                    inspect("value", trait.valueType(), mValue.get(), readOnly);
                    ImGui::EndTable();

                    if (mKeyExists)
                    {
                        ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.00F), "Key already exists.");
                    }

                    if (ImGui::Button("Add", ImVec2(80, 0)))
                    {
                        if (view.find(mKey.get()) != view.end())
                        {
                            mKeyExists = true;
                        }
                        else
                        {
                            mKeyExists = false;
                            view.insertMove(mKey.get(), mValue.get());
                            changed = true;
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Cancel", ImVec2(80, 0)))
                    {
                        mKeyExists = false;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }
        });
    }
    else if (type.has<FieldsTrait>())
    {
        const auto& trait = type.get<FieldsTrait>();
        auto view = trait.view(value);
        if (trait.size() == 1)
        {
            auto [field, fieldVal] = *view.begin();
            changed |= inspect(name, field->type(), fieldVal, readOnly);
        }
        else
        {
            withStructured(name, type, [&]() {
                for (auto [field, fieldVal] : view)
                {
                    changed |= inspect(field->name(), field->type(), fieldVal, readOnly);
                }
            });
        }
    }
    else if (type.has<StringConversionTrait>())
    {
        const auto& stringConversion = type.get<StringConversionTrait>();
        auto str = stringConversion.into(value);
        if (readOnly)
        {
            showKnown(name, type, static_cast<void*>(&str));
        }
        else
        {
            changed |= editKnown(name, type, static_cast<void*>(&str));
            if (changed)
            {
                stringConversion.from(value, str);
            }
        }
    }
    else
    {
        if (readOnly)
        {
            showKnown(name, type, value);
        }
        else
        {
            changed |= editKnown(name, type, value);
        }
    }

    return changed;
}

/// @brief Returns a string represeting primitive value, depending on it's [TYPE].
#define RETURN_FORMAT_STRING_IF_TYPE(TYPE)                                                                             \
    if (type.is<TYPE>())                                                                                               \
    {                                                                                                                  \
        return std::to_string(*static_cast<const TYPE*>(value));                                                       \
    }

std::string DataInspector::stringKnown(const void* value, const Type& type)
{
    RETURN_FORMAT_STRING_IF_TYPE(int8_t)
    RETURN_FORMAT_STRING_IF_TYPE(int8_t)
    RETURN_FORMAT_STRING_IF_TYPE(int32_t)
    RETURN_FORMAT_STRING_IF_TYPE(uint8_t)
    RETURN_FORMAT_STRING_IF_TYPE(uint16_t)
    RETURN_FORMAT_STRING_IF_TYPE(uint32_t)
    RETURN_FORMAT_STRING_IF_TYPE(uint64_t)
    RETURN_FORMAT_STRING_IF_TYPE(float)
    RETURN_FORMAT_STRING_IF_TYPE(double)
    if (type.is<bool>())
    {
        return *static_cast<const bool*>(value) ? "true" : "false";
    }
    if (type.is<std::string>())
    {
        mTextBuffer = *static_cast<const std::string*>(value);
        return mTextBuffer;
    }

    CUBOS_ERROR("Expected value with primitive type, found unsupported type '{}'", type.name());
    return {"???"};
}

void DataInspector::showKnown(const std::string& name, const Type& type, const void* value)
{
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::TreeNodeEx(name.c_str(),
                      ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("%s", type.name().c_str());
    }

    std::string str = stringKnown(value, type);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(str.c_str());
}

void DataInspector::withStructured(const std::string& name, const Type& type, auto fn)
{
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool nodeOpen = ImGui::TreeNode(name.c_str());
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("%s", type.name().c_str());
    }

    if (nodeOpen)
    {
        fn();
        ImGui::TreePop();
    }
}

#define GET_INPUT_SCALAR_IF_TYPE(TYPE, DATATYPE)                                                                       \
    if (type.is<TYPE>())                                                                                               \
    {                                                                                                                  \
        supported = true;                                                                                              \
        ImGui::PushID(name.c_str());                                                                                   \
        if (ImGui::InputScalar("", DATATYPE, value))                                                                   \
        {                                                                                                              \
            modified = true;                                                                                           \
        }                                                                                                              \
        ImGui::PopID();                                                                                                \
    }

bool DataInspector::editKnown(const std::string& name, const Type& type, void* value)
{
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::TreeNodeEx(name.c_str(),
                      ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
    {
        ImGui::SetTooltip("%s", type.name().c_str());
    }

    ImGui::PushID("editKnown");
    ImGui::TableSetColumnIndex(1);
    bool modified = false;
    bool supported = false;
    GET_INPUT_SCALAR_IF_TYPE(int8_t, ImGuiDataType_S8)
    GET_INPUT_SCALAR_IF_TYPE(int16_t, ImGuiDataType_S16)
    GET_INPUT_SCALAR_IF_TYPE(int32_t, ImGuiDataType_S32)
    GET_INPUT_SCALAR_IF_TYPE(int64_t, ImGuiDataType_S64)
    GET_INPUT_SCALAR_IF_TYPE(uint8_t, ImGuiDataType_U8)
    GET_INPUT_SCALAR_IF_TYPE(uint16_t, ImGuiDataType_U16)
    GET_INPUT_SCALAR_IF_TYPE(uint32_t, ImGuiDataType_U32)
    GET_INPUT_SCALAR_IF_TYPE(uint64_t, ImGuiDataType_U64)
    GET_INPUT_SCALAR_IF_TYPE(float, ImGuiDataType_Float)
    GET_INPUT_SCALAR_IF_TYPE(double, ImGuiDataType_Double)
    if (type.is<bool>())
    {
        supported = true;
        if (ImGui::Checkbox(name.c_str(), static_cast<bool*>(value)))
        {
            modified = true;
        }
    }
    if (type.is<std::string>())
    {
        supported = true;
        mTextBuffer = *static_cast<const std::string*>(value);
        ImGui::PushID(name.c_str());
        if (ImGui::InputText("", &mTextBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            auto* p = static_cast<std::string*>(value);
            *p = mTextBuffer;
            modified = true;
        }
        ImGui::PopID();
    }
    if (!supported)
    {
        ImGui::Text("???");
        CUBOS_ERROR("Type {} is not supported by the Data Inspector.", type.name());
    }

    ImGui::PopID();

    return modified;
}
