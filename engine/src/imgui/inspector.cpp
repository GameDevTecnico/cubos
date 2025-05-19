#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <imgui.h>
#include <imgui_stdlib.h>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constant.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/traits/vector.hpp>
#include <cubos/core/reflection/traits/wrapper.hpp>

#include <cubos/engine/imgui/inspector.hpp>

using cubos::core::ecs::World;
using cubos::core::memory::AnyValue;
using cubos::engine::ImGuiInspector;

using namespace cubos::core::reflection;

CUBOS_REFLECT_IMPL(ImGuiInspector::State)
{
    return core::ecs::TypeBuilder<ImGuiInspector::State>("cubos::engine::ImGuiInspector::State").build();
}

ImGuiInspector::ImGuiInspector(World& world, State& state)
    : mWorld{world}
    , mState{state}
{
}

void ImGuiInspector::show(const std::string& name, const core::reflection::Type& type, const void* value)
{
    auto modified = inspect(name, true, type, const_cast<void*>(value));
    CUBOS_ASSERT(!modified, "Read only values should not be modifiable");
}

bool ImGuiInspector::edit(const std::string& name, const core::reflection::Type& type, void* value)
{
    return this->inspect(name, false, type, value);
}

bool ImGuiInspector::inspect(const std::string& name, bool readOnly, const core::reflection::Type& type, void* value)
{
    for (std::size_t i = mState.hooks.size(); i > 0; --i)
    {
        auto& hook = mState.hooks[i - 1];
        switch (hook(name, readOnly, *this, type, value))
        {
        case HookResult::Shown:
            return false;
        case HookResult::Modified:
            if (readOnly)
            {
                CUBOS_FAIL("Inspector hooks should not modify read-only values");
            }
            return true;
        case HookResult::Unhandled:
            continue; // Default to the next hook.
        }
    }

    CUBOS_UNREACHABLE("Default hook should catch all cases");
}

void ImGuiInspector::hook(Hook hook)
{
    mState.hooks.emplace_back(std::move(hook));
}

ImGuiInspector::State::State()
{
    // Catch all unhandled types.
    hooks.emplace_back([](const std::string& name, bool, ImGuiInspector&, const Type& type, void*) {
        std::string label = "unsupported";
        ImGui::BeginDisabled(true);
        ImGui::InputText(name.c_str(), label.data(), label.size());
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Inspector does not know how to handle the type %s", type.shortName().c_str());
        }

        return ImGuiInspector::HookResult::Shown;
    });

    // Catch all primitive types.
    hooks.emplace_back([](const std::string& name, bool readOnly, ImGuiInspector&, const Type& type, void* value) {
        ImGui::BeginDisabled(readOnly);
        bool modified = false;

        if (type.is<bool>())
        {
            modified |= ImGui::Checkbox(name.c_str(), static_cast<bool*>(value));
        }
        else if (type.is<char>())
        {
            char buf[2] = {*static_cast<char*>(value), '\0'};
            modified |= ImGui::InputText(name.c_str(), buf, sizeof(buf));
            if (modified)
            {
                *static_cast<char*>(value) = buf[0];
            }
        }
        else if (type.is<signed char>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_S8, value);
        }
        else if (type.is<short>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_S16, value);
        }
        else if (type.is<int>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_S32, value);
        }
        else if (type.is<long>())
        {
            long long temp = *static_cast<long*>(value);
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_S64, &temp);
            if (modified)
            {
                *static_cast<long*>(value) = static_cast<long>(temp);
            }
        }
        else if (type.is<long long>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_S64, value);
        }
        else if (type.is<unsigned char>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_U8, value);
        }
        else if (type.is<unsigned short>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_U16, value);
        }
        else if (type.is<unsigned int>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, value);
        }
        else if (type.is<unsigned long>())
        {
            unsigned long long temp = *static_cast<unsigned long*>(value);
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_U64, &temp);
            if (modified)
            {
                *static_cast<unsigned long*>(value) = static_cast<unsigned long>(temp);
            }
        }
        else if (type.is<unsigned long long>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_U64, value);
        }
        else if (type.is<float>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_Float, value);
        }
        else if (type.is<double>())
        {
            modified |= ImGui::InputScalar(name.c_str(), ImGuiDataType_Double, value);
        }
        else
        {
            ImGui::EndDisabled();
            return ImGuiInspector::HookResult::Unhandled;
        }
        ImGui::EndDisabled();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Primitive of type %s", type.shortName().c_str());
        }

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle object types.
    hooks.emplace_back(
        [](const std::string& name, bool readOnly, ImGuiInspector& inspector, const Type& type, void* value) {
            if (!type.has<FieldsTrait>())
            {
                return ImGuiInspector::HookResult::Unhandled;
            }

            bool open = ImGui::CollapsingHeader(name.c_str());
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Object of type %s", type.shortName().c_str());
            }
            if (!open)
            {
                return ImGuiInspector::HookResult::Shown;
            }

            ImGui::PushID(name.c_str());
            ImGui::Indent();

            const auto& fields = type.get<FieldsTrait>();
            bool modified = false;
            for (const auto& [field, fieldValue] : fields.view(value))
            {
                modified |= inspector.inspect(field->name(), readOnly, field->type(), fieldValue);
            }

            ImGui::Unindent();
            ImGui::PopID();

            return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
        });

    // Handle array types.
    hooks.emplace_back(
        [](const std::string& name, bool readOnly, ImGuiInspector& inspector, const Type& type, void* value) {
            if (!type.has<ArrayTrait>())
            {
                return ImGuiInspector::HookResult::Unhandled;
            }

            bool open = ImGui::CollapsingHeader(name.c_str());
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Array of type %s", type.shortName().c_str());
            }
            if (!open)
            {
                return ImGuiInspector::HookResult::Shown;
            }

            ImGui::PushID(name.c_str());
            ImGui::Indent();

            const auto& array = type.get<ArrayTrait>();
            auto arrayView = array.view(value);
            bool modified = false;
            std::size_t eraseI = arrayView.length();
            for (std::size_t i = 0; i < arrayView.length(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));

                if (!readOnly && array.hasErase())
                {
                    if (ImGui::Button("X"))
                    {
                        eraseI = i;
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Remove element %zu", i);
                    }
                    ImGui::SameLine();
                }

                modified |= inspector.inspect(std::to_string(i), readOnly, array.elementType(), arrayView.get(i));

                ImGui::PopID();
            }

            if (eraseI < arrayView.length())
            {
                arrayView.erase(eraseI);
                modified = true;
            }

            if (!readOnly)
            {
                if (array.hasInsertDefault())
                {
                    if (ImGui::Button("Add"))
                    {
                        arrayView.insertDefault(arrayView.length());
                        modified = true;
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Add new default valued element");
                    }
                }
                if (array.hasInsertDefault() && array.hasErase())
                {
                    ImGui::SameLine();
                }
                if (array.hasErase())
                {
                    if (ImGui::Button("Clear"))
                    {
                        arrayView.clear();
                        modified = true;
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Remove all elements");
                    }
                }
            }

            ImGui::Unindent();
            ImGui::PopID();

            return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
        });

    // Handle dictionary types.
    hooks.emplace_back([newKey = AnyValue{}](const std::string& name, bool readOnly, ImGuiInspector& inspector,
                                             const Type& type, void* value) mutable {
        if (!type.has<DictionaryTrait>())
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        bool open = ImGui::CollapsingHeader(name.c_str());
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Dictionary of type %s", type.shortName().c_str());
        }
        if (!open)
        {
            return ImGuiInspector::HookResult::Shown;
        }

        ImGui::PushID(name.c_str());
        ImGui::Indent();

        const auto& dictionary = type.get<DictionaryTrait>();
        auto dictionaryView = dictionary.view(value);
        bool modified = false;
        DictionaryTrait::View::Iterator eraseIt = dictionaryView.end();

        if (ImGui::BeginTable("Dictionary", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
        {
            int i = 0;

            for (auto it = dictionaryView.begin(); it != dictionaryView.end(); ++it)
            {
                const auto& [entryKey, entryValue] = *it;

                ImGui::PushID(static_cast<int>(i++));
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (!readOnly && dictionary.hasErase())
                {
                    if (ImGui::Button("X"))
                    {
                        eraseIt = it;
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Remove entry");
                    }
                    ImGui::SameLine();
                }
                inspector.show("Key", dictionary.keyType(), entryKey);
                ImGui::TableNextColumn();
                modified |= inspector.inspect("Value", readOnly, dictionary.valueType(), entryValue);
                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        if (eraseIt != dictionaryView.end())
        {
            dictionaryView.erase(eraseIt);
            modified = true;
        }

        if (!readOnly)
        {
            bool canInsert = dictionary.hasInsertDefault() && dictionary.keyType().has<ConstructibleTrait>() &&
                             dictionary.keyType().get<ConstructibleTrait>().hasDefaultConstruct();
            if (canInsert)
            {
                if (ImGui::Button("Add"))
                {
                    ImGui::OpenPopup("Add dictionary entry");
                    newKey = AnyValue::defaultConstruct(dictionary.keyType());
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Add new default valued entry");
                }

                if (ImGui::BeginPopup("Add dictionary entry"))
                {
                    inspector.edit("Key", dictionary.keyType(), newKey.get());
                    ImGui::Separator();
                    if (ImGui::Button("OK"))
                    {
                        dictionaryView.insertDefault(newKey.get());
                        modified = true;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            if (canInsert && dictionary.hasErase())
            {
                ImGui::SameLine();
            }
            if (dictionary.hasErase())
            {
                if (ImGui::Button("Clear"))
                {
                    dictionaryView.clear();
                    modified = true;
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Remove all entries");
                }
            }
        }

        ImGui::Unindent();
        ImGui::PopID();

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle string conversion types.
    hooks.emplace_back([](const std::string& name, bool readOnly, ImGuiInspector&, const Type& type, void* value) {
        if (!type.has<StringConversionTrait>())
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        const auto& stringConversion = type.get<StringConversionTrait>();
        std::string str = stringConversion.into(value);
        ImGui::BeginDisabled(readOnly);
        bool modified = ImGui::InputText(name.c_str(), &str);
        if (modified)
        {
            if (!stringConversion.from(value, str))
            {
                ImGui::OpenPopup("String conversion error");
            }
        }
        ImGui::EndDisabled();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("String representation of type %s", type.shortName().c_str());
        }

        if (ImGui::BeginPopup("String conversion error"))
        {
            ImGui::TextColored({1.0F, 0.0F, 0.0F, 1.0F}, "String is not a valid representation of the type %s",
                               type.shortName().c_str());
            ImGui::EndPopup();
        }

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle enum types.
    hooks.emplace_back([](const std::string& name, bool readOnly, ImGuiInspector&, const Type& type, void* value) {
        if (!type.has<EnumTrait>())
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        const auto& enumTrait = type.get<EnumTrait>();
        ImGui::BeginDisabled(readOnly);
        bool modified = false;
        if (ImGui::BeginCombo(name.c_str(), enumTrait.variant(value).name().c_str()))
        {
            for (const auto& variant : enumTrait)
            {
                bool selected = variant.test(value);
                if (ImGui::Selectable(variant.name().c_str(), selected))
                {
                    variant.set(value);
                    modified = true;
                }
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Enum of type %s", type.shortName().c_str());
        }

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle mask types.
    hooks.emplace_back([](const std::string& name, bool readOnly, ImGuiInspector&, const Type& type, void* value) {
        if (!type.has<MaskTrait>())
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        const auto& mask = type.get<MaskTrait>();
        std::string maskStr;
        for (const auto& bit : mask.view(value))
        {
            if (!maskStr.empty())
            {
                maskStr += " + ";
            }
            maskStr += bit.name();
        }
        if (maskStr.empty())
        {
            maskStr = "None";
        }

        bool modified = false;
        if (ImGui::BeginCombo(name.c_str(), maskStr.c_str()))
        {
            for (const auto& bit : mask)
            {
                bool selected = bit.test(value);
                ImGui::BeginDisabled(readOnly);
                if (ImGui::Checkbox(bit.name().c_str(), &selected))
                {
                    if (selected)
                    {
                        bit.set(value);
                    }
                    else
                    {
                        bit.clear(value);
                    }
                    modified = true;
                }
                ImGui::EndDisabled();
            }

            if (!readOnly)
            {
                ImGui::Separator();
                if (ImGui::Button("None"))
                {
                    for (const auto& bit : mask)
                    {
                        bit.clear(value);
                    }
                    modified = true;
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Clear all bits");
                }
                ImGui::SameLine();
                if (ImGui::Button("All"))
                {
                    for (const auto& bit : mask)
                    {
                        bit.set(value);
                    }
                    modified = true;
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Set all bits");
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Mask of type %s", type.shortName().c_str());
        }

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle primitive vector types.
    hooks.emplace_back([](const std::string& name, bool readOnly, ImGuiInspector&, const Type& type, void* value) {
        if (!type.has<VectorTrait>())
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        const auto& vector = type.get<VectorTrait>();
        ImGuiDataType dataType;
        if (vector.scalarType().is<float>())
        {
            dataType = ImGuiDataType_Float;
        }
        else if (vector.scalarType().is<double>())
        {
            dataType = ImGuiDataType_Double;
        }
        else if (vector.scalarType().is<int>())
        {
            dataType = ImGuiDataType_S32;
        }
        else if (vector.scalarType().is<unsigned int>())
        {
            dataType = ImGuiDataType_U32;
        }
        else
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        ImGui::BeginDisabled(readOnly);
        bool modified = ImGui::InputScalarN(name.c_str(), dataType, value, static_cast<int>(vector.dimensions()));
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Vector of type %s", type.shortName().c_str());
        }

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle GLM quaternion types.
    hooks.emplace_back([](const std::string& name, bool readOnly, ImGuiInspector&, const Type& type, void* value) {
        bool modified = false;

        if (type.is<glm::quat>())
        {
            glm::quat& quat = *static_cast<glm::quat*>(value);
            auto euler = glm::degrees(glm::eulerAngles(quat));

            ImGui::BeginDisabled(readOnly);
            if (ImGui::InputScalarN(name.c_str(), ImGuiDataType_Float, &euler, 3))
            {
                quat = glm::normalize(glm::quat(glm::radians(euler)));
                modified = true;
            }
            ImGui::EndDisabled();
        }
        else if (type.is<glm::dquat>())
        {
            glm::dquat& quat = *static_cast<glm::dquat*>(value);
            auto euler = glm::degrees(glm::eulerAngles(quat));

            ImGui::BeginDisabled(readOnly);
            if (ImGui::InputScalarN(name.c_str(), ImGuiDataType_Double, &euler, 3))
            {
                quat = glm::normalize(glm::quat(glm::radians(euler)));
                modified = true;
            }
            ImGui::EndDisabled();
        }
        else
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Quaternion of type %s (in XYZ euler angles)", type.shortName().c_str());
        }

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle 4x4 GLM matrices.
    hooks.emplace_back([decompose = false](const std::string& name, bool readOnly, ImGuiInspector& inspector,
                                           const Type& type, void* value) mutable {
        if (!type.is<glm::mat4>() && !type.is<glm::dmat4>())
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        auto mat = type.is<glm::mat4>() ? static_cast<glm::dmat4>(*static_cast<glm::mat4*>(value))
                                        : *static_cast<glm::dmat4*>(value);

        glm::dvec3 scale;
        glm::dquat orientation;
        glm::dvec3 translation;
        glm::dvec3 skew;
        glm::dvec4 perspective;
        bool canDecompose = decompose && glm::decompose(mat, scale, orientation, translation, skew, perspective);

        bool modified = false;
        bool open = ImGui::CollapsingHeader(name.c_str());
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Matrix of type %s", type.shortName().c_str());
        }
        if (open)
        {
            ImGui::PushID(name.c_str());
            ImGui::Indent();

            if (canDecompose)
            {
                modified |= inspector.inspect("scale", readOnly, reflect<glm::dvec3>(), &scale);
                modified |= inspector.inspect("rotation", readOnly, reflect<glm::dquat>(), &orientation);
                modified |= inspector.inspect("translation", readOnly, reflect<glm::dvec3>(), &translation);
                inspector.show("skew", skew);
                inspector.show("perspective", perspective);
                if (modified)
                {
                    mat = glm::translate(glm::dmat4(1.0F), translation) * glm::toMat4(orientation) *
                          glm::scale(glm::dmat4(1.0F), scale);
                }
            }
            else
            {
                modified |= inspector.inspect("a", readOnly, reflect<glm::dvec4>(), &mat[0]);
                modified |= inspector.inspect("b", readOnly, reflect<glm::dvec4>(), &mat[1]);
                modified |= inspector.inspect("c", readOnly, reflect<glm::dvec4>(), &mat[2]);
                modified |= inspector.inspect("d", readOnly, reflect<glm::dvec4>(), &mat[3]);
            }

            auto justChangedDecompose = ImGui::Checkbox("Decompose into components", &decompose);
            if (decompose && !canDecompose && !justChangedDecompose)
            {
                ImGui::TextColored({1.0F, 0.0F, 0.0F, 1.0F}, "Matrix cannot be decomposed");
            }

            if (modified)
            {
                if (type.is<glm::mat4>())
                {
                    *static_cast<glm::mat4*>(value) = static_cast<glm::mat4>(mat);
                }
                else
                {
                    *static_cast<glm::dmat4*>(value) = mat;
                }
            }

            ImGui::Unindent();
            ImGui::PopID();
        }

        return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
    });

    // Handle wrapper types.
    hooks.emplace_back(
        [](const std::string& name, bool readOnly, ImGuiInspector& inspector, const Type& type, void* value) {
            if (!type.has<WrapperTrait>())
            {
                return ImGuiInspector::HookResult::Unhandled;
            }

            const auto& wrapper = type.get<WrapperTrait>();
            bool modified = inspector.inspect(name, readOnly, wrapper.type(), wrapper.value(value));
            return modified ? ImGuiInspector::HookResult::Modified : ImGuiInspector::HookResult::Shown;
        });

    // Catch constant types.
    hooks.emplace_back([](const std::string& name, bool, ImGuiInspector& inspector, const Type& type, void* value) {
        if (!type.has<ConstantTrait>())
        {
            return ImGuiInspector::HookResult::Unhandled;
        }

        const auto& constant = type.get<ConstantTrait>();
        inspector.show(name, constant.type(), value);
        return ImGuiInspector::HookResult::Shown;
    });
}
