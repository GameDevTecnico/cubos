#ifndef CUBOS_CORE_UI_SERIALIZATION_HPP
#define CUBOS_CORE_UI_SERIALIZATION_HPP

#include <cubos/core/data/package.hpp>

namespace cubos::core::ui
{
    /// Shows a serializable object's properties in the UI. Should be called
    /// inside a ImGui::Begin()/ImGui::End() block.
    /// @tparam T The type of the serializable object.
    /// @param object The object to show.
    /// @param name The name of the object.
    template <typename T>
    requires data::TriviallySerializable<T>
    void show(const T& object, const std::string& name);

    /// Shows a serializable and deserializable object's properties in the UI,
    /// allowing the user to edit the object. If any of the properties is
    /// edited, the object's deserialize() method is called. Should be called
    /// inside a ImGui::Begin()/ImGui::End() block.
    /// @tparam T The type of the serializable object.
    /// @param object The object to edit.
    /// @param name The name of the object.
    /// @returns True if the object was edited, false otherwise.
    template <typename T>
    requires data::TriviallySerializable<T> && data::TriviallyDeserializable<T>
    bool edit(T& object, const std::string& name);

    /// Shows a packaged object's properties in the UI. Should be called
    /// inside a ImGui::Begin()/ImGui::End() block.
    /// @param pkg The packaged object to show.
    /// @param name The name of the object.
    void showPackage(const data::Package& pkg, const std::string& name);

    /// Shows a packaged object's properties in the UI, allowing the user to
    /// edit the object.
    /// @param pkg The packaged object to edit.
    /// @param name The name of the object.
    /// @returns True if the object was edited, false otherwise.
    bool editPackage(data::Package& pkg, const std::string& name);

    // Implementation of the above functions.

    template <typename T>
    requires data::TriviallySerializable<T>
    void show(const T& object, const std::string& name)
    {
        auto pkg = data::Package::from(object);
        showPackage(pkg, name);
    }

    template <typename T>
    requires data::TriviallySerializable<T> && data::TriviallyDeserializable<T>
    bool edit(T& object, const std::string& name)
    {
        auto pkg = data::Package::from(object);
        if (editPackage(pkg, name))
        {
            pkg.into(object);
            return true;
        }
        else
        {
            return false;
        }
    }

} // namespace cubos::core::ui

#endif // CUBOS_CORE_UI_SERIALIZATION_HPP
