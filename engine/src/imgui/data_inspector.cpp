
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/imgui/data_inspector.hpp>

using cubos::core::memory::AnyValue;
using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(DataInspector)
{
    return core::ecs::TypeBuilder<DataInspector>("cubos::engine::DataInspector").build();
}

/// @brief Displays a menu where the user can "set" the data to null (if it has NullableTrait)
static bool nullifyMenu(const Type& type, void* value);

void DataInspector::show(const core::reflection::Type& type, const void* value)
{
    if (ImGui::BeginTable("inspector", 2))
    {
        this->inspect("", type, const_cast<void*>(value), true);
        ImGui::EndTable();
    }
}

bool DataInspector::edit(const core::reflection::Type& type, void* value)
{
    if (ImGui::BeginTable("inspector", 2))
    {
        auto changed = this->inspect("", type, value, false);
        ImGui::EndTable();
        return changed;
    }

    return false;
}

bool DataInspector::inspect(const std::string& name, const Type& type, void* value, bool readOnly)
{
    ImGui::TableNextRow();
    bool changed = false;

    if (type.has<NullableTrait>())
    {
        const auto& trait = type.get<NullableTrait>();
        if (trait.isNull(value))
        {
            showKnown(name, type, nullptr);
            if (!readOnly)
            {
                ImGui::SameLine();
                ImGui::BeginDisabled(!type.has<ConstructibleTrait>());

                bool reset = ImGui::Button("Reset");
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    if (!type.has<ConstructibleTrait>())
                    {
                        ImGui::SetTooltip("Type does not have ConstructibleTrait");
                    }
                    else
                    {
                        ImGui::SetTooltip("Construct new data");
                    }
                }

                if (reset)
                {
                    const auto& constructible = type.get<ConstructibleTrait>();
                    constructible.defaultConstruct(value);
                    changed = true;
                }

                ImGui::EndDisabled();
            }

            return changed;
        }
    }

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
                    std::string idxStrRepresentation = "[";
                    idxStrRepresentation.append(std::to_string(idx));
                    idxStrRepresentation.push_back(']');
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
                    if (ImGui::BeginTable("DictPopupInsert", 2))
                    {
                        inspect("key", trait.keyType(), mKey.get(), readOnly);
                        inspect("value", trait.valueType(), mValue.get(), readOnly);
                        ImGui::EndTable();
                    }

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
        if (trait.size() == 0)
        {
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted("(no fields)");
        }
        else if (trait.size() == 1)
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
            showKnown(name, core::reflection::reflect<decltype(str)>(), static_cast<void*>(&str));
        }
        else
        {
            changed |= editKnown(name, core::reflection::reflect<decltype(str)>(), static_cast<void*>(&str));
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

    if (!readOnly)
    {
        changed |= nullifyMenu(type, value);
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
    if (value == nullptr)
    {
        return {"null"};
    }
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

    return "(unsupported)";
}

void DataInspector::showKnown(const std::string& name, const Type& type, const void* value)
{

    if (!name.empty())
    {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TreeNodeEx(name.c_str(),
                          ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("%s", type.name().c_str());
        }
    }

    std::string str = stringKnown(value, type);
    ImGui::TableSetColumnIndex(1);
    ImGui::TextUnformatted(str.c_str());
}

void DataInspector::withStructured(const std::string& name, const Type& type, auto fn)
{
    if (!name.empty())
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
    else
    {
        fn();
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
    if (!name.empty())
    {
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TreeNodeEx(name.c_str(),
                          ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("%s", type.name().c_str());
        }
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
        ImGui::PushID(name.c_str());
        if (ImGui::Checkbox("", static_cast<bool*>(value)))
        {
            modified = true;
        }
        ImGui::PopID();
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
        ImGui::Text("(unsupported)");
    }

    ImGui::PopID();

    return modified;
}

static bool nullifyMenu(const Type& type, void* value)
{
    bool changed = false;

    if (type.has<NullableTrait>())
    {
        const auto& trait = type.get<NullableTrait>();
        if (!trait.isNull(value))
        {
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::Button("Nullify"))
                {
                    trait.setToNull(value);
                    changed = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }

    return changed;
}