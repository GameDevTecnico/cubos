/// @file
/// @brief Class @ref cubos::engine::TransparentEqual.
/// @ingroup input-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <string>

namespace cubos::engine
{
    /// @brief Redefines equality function for std::string_view and std::string.
    ///
    /// This class is used to allow heterogeneous lookup in unordered maps and sets.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputTransparentEqual final
    {
    public:
        CUBOS_REFLECT;
        
        using IsTransparent = void; // Tag to allow heterogeneous lookup

        ~InputTransparentEqual() = default;
        InputTransparentEqual() = default;

        /// @brief Compares two string views for equality.
        /// @param svhs The first string view.
        /// @param svvhs The second string view.
        /// @return True if the string views are equal, false otherwise.
        bool operator()(std::string_view svhs, std::string_view svvhs) const;
    
        /// @brief Compares a string and a string view for equality.
        /// @param shs The string.
        /// @param svhs The string view.
        /// @return True if the string and string view are equal, false otherwise.
        bool operator()(const std::string& shs, std::string_view svhs) const;
    
        /// @brief Compares a string view and a string for equality.
        /// @param svhs The string view.
        /// @param shs The string.
        /// @return True if the string view and string are equal, false otherwise.
        bool operator()(std::string_view svhs, const std::string& shs) const;

        /// @brief Compares two strings for equality.
        /// @param shs The first string.
        /// @param sshs The second string.
        /// @return True if the strings are equal, false otherwise.
        bool operator()(const std::string& shs, const std::string& sshs) const;

    };
} // namespace cubos::engine


