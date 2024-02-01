/// @file
/// @brief Resource @ref cubos::engine::DataInspector.
/// @ingroup imgui-plugin

#pragma once

#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/type.hpp>

namespace cubos::engine
{
    /// @brief Resource which allows the user to inspect or modify any reflectable value on the UI.
    /// @see Take a look at the @ref examples-engine-imgui example for a demonstration of this resource.
    /// @ingroup imgui-plugin
    class DataInspector final
    {
    public:
        /// @brief Displays a reflectable value on the UI.
        /// @param type Value type.
        /// @param value Pointer to value.
        void show(const core::reflection::Type& type, const void* value);

        /// @brief Displays a reflectable value on the UI and allows modifying it.
        /// @param type Value type.
        /// @param value Pointer to value.
        /// @return Whether the object was modified.
        bool edit(const core::reflection::Type& type, void* value);

        /// @copybrief show
        /// @tparam T Value type.
        /// @param name Value name.
        /// @param value Pointer to value.
        template <typename T>
        void show(const T& value)
        {
            this->show(core::reflection::reflect<T>(), &value);
        }

        /// @copybrief edit
        /// @tparam T Value type.
        /// @param value Pointer to value.
        /// @return Whether the object was modified.
        template <typename T>
        bool edit(T& value)
        {
            return this->edit(core::reflection::reflect<T>(), &value);
        }

    private:
        /// @brief Inspect a reflective object.
        /// @note This should always be called inside a `ImGui::BeginTable(..., 2)` and `ImGui::EndTable()`.
        /// @param name Value name.
        /// @param type Value type.
        /// @param value Pointer to value.
        /// @param readOnly A flag that indicates whether this field can be modified.
        /// @return True if object was modified.
        bool inspect(const std::string& name, const core::reflection::Type& type, void* value, bool readOnly = true);

        /// @brief Displays a value of a known type, or "???" if its unknown. Allows modification.
        /// @param name Value name.
        /// @param type Value type.
        /// @param value Pointer to value.
        /// @return True if value was succesfully modified.
        bool editKnown(const std::string& name, const core::reflection::Type& type, void* value);

        /// @brief Gets a representative string for a value of a known type, or "???" if its unknown.
        /// @param name Value name.
        /// @param type Value type.
        /// @param value Pointer to value.
        std::string stringKnown(const void* value, const core::reflection::Type& type);

        /// @brief Displays a value of a known type, or "???" if its unknown.
        /// @param name Value name.
        /// @param type Value type.
        /// @param value Pointer to value.
        void showKnown(const std::string& name, const core::reflection::Type& type, const void* value);

        /// @brief Displays a structured value inside a `ImGui::TreeNode`.
        /// @param name Value name.
        /// @param type Value type.
        /// @param fn Functor called when the node is open.
        void withStructured(const std::string& name, const core::reflection::Type& type, auto fn);

        std::string mTextBuffer;
        bool mKeyExists;
        cubos::core::memory::AnyValue mKey;
        cubos::core::memory::AnyValue mValue;
    };

} // namespace cubos::engine
