/// @file
/// @brief Functions for showing and editing serializable objects in the UI.
/// @ingroup core-ui

#pragma once

#include <cubos/core/data/package.hpp>

namespace cubos::core::ui
{
    /// @brief Shows a packaged object's properties in the UI. Should be called inside a
    /// `ImGui::BeginTable(2)` and `ImGui::EndTable()` block.
    ///
    /// The first column displays the name of the package and the second column displays the value
    /// of the package.
    ///
    /// @param pkg Packaged object to show.
    /// @param name Name of the object.
    /// @ingroup core-ui
    void showPackage(const data::Package& pkg, const std::string& name);

    /// @brief Shows a packaged object's properties in the UI, allowing the user to edit the
    /// object. Should be called inside a `ImGui::BeginTable(3)` and `ImGui::EndTable()` block.
    ///
    /// The first column displays the name of the package, the second column displays the value
    /// of the package and the third column is used for remove buttons in the case of arrays and
    /// dictionaries.
    ///
    /// @param pkg Packaged object to edit.
    /// @param name Name of the object.
    /// @return True if the object was modified, false otherwise.
    /// @ingroup core-ui
    bool editPackage(data::Package& pkg, const std::string& name);

    /// @brief Shows a serializable object's properties in the UI. Should be called inside a
    /// `ImGui::BeginTable(2)` and `ImGui::EndTable()` block.
    ///
    /// Internally packages the object and calls @ref showPackage().
    ///
    /// @tparam T Type of the serializable object.
    /// @param object Object to show.
    /// @param name Name of the object.
    /// @ingroup core-ui
    template <typename T>
    inline void show(const T& object, const std::string& name)
    {
        auto pkg = data::Package::from(object);
        showPackage(pkg, name);
    }

    /// @brief Shows a serializable object's properties in the UI. Should be called inside a
    /// `ImGui::BeginTable(3)` and `ImGui::EndTable()` block.
    ///
    /// Internally packages the object, calls @ref editPackage() and then unpackages the object, if
    /// it was modified.
    ///
    /// @tparam T Type of the serializable object.
    /// @param object Object to edit.
    /// @param name Name of the object.
    /// @return True if the object was edited, false otherwise.
    /// @ingroup core-ui
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
