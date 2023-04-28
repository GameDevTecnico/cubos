#ifndef CUBOS_CORE_UI_SERIALIZATION_HPP
#define CUBOS_CORE_UI_SERIALIZATION_HPP

#include <cubos/core/data/package.hpp>

namespace cubos::core::ui
{
    /// Shows a packaged object's properties in the UI. Should be called
    /// inside a ImGui::BeginTable(2)/ImGui::EndTable() block.
    ///
    /// The first column displays the name of the package and the second column displays the value of the package
    /// @param pkg The packaged object to show.
    /// @param name The name of the object.
    void showPackage(const data::Package& pkg, const std::string& name);

    /// Shows a packaged object's properties in the UI, allowing the user to
    /// edit the object. Should be called inside a
    /// ImGui::BeginTable(3)/ImGui::EndTable() block.
    ///
    /// The first column displays the name of the package, the second column displays the value of the package, and the
    /// third column is used for remove buttons in the case of arrays and dictionaries.
    /// @param pkg The packaged object to edit.
    /// @param name The name of the object.
    /// @returns True if the object was edited, false otherwise.
    bool editPackage(data::Package& pkg, const std::string& name);

    /// Shows a serializable object's properties in the UI. Should be called
    /// inside a ImGui::BeginTable(2)/ImGui::EndTable() block.
    /// @tparam T The type of the serializable object.
    /// @param object The object to show.
    /// @param name The name of the object.
    template <typename T>
    inline void show(const T& object, const std::string& name)
    {
        auto pkg = data::Package::from(object);
        showPackage(pkg, name);
    }

    /// Shows a serializable and deserializable object's properties in the UI,
    /// allowing the user to edit the object. If any of the properties is
    /// edited, the object's deserialize() method is called. Should be called
    /// inside a ImGui::BeginTable(3)/ImGui::EndTable() block.
    /// @tparam T The type of the serializable object.
    /// @param object The object to edit.
    /// @param name The name of the object.
    /// @returns True if the object was edited, false otherwise.
    template <typename T>
    inline bool edit(T& object, const std::string& name)
    {
        auto pkg = data::Package::from(object);
        if (editPackage(pkg, name))
        {
            pkg.into(object);
            return true;
        }

        return false;
    }
} // namespace cubos::core::ui

#endif // CUBOS_CORE_UI_SERIALIZATION_HPP
