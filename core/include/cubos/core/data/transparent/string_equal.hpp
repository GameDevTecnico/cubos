/// @file
/// @brief Class @ref cubos::core::data::StringTransparentEqual.
/// @ingroup core-data-transparent

#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/api.hpp>
#include <string>

namespace cubos::core::data
{
    /// @brief Redefines equality function for std::string_view and std::string.
    ///
    /// This class is used to allow heterogeneous lookup in unordered maps and sets.
    ///
    /// @ingroup core-data-transparent
    class CUBOS_CORE_API StringTransparentEqual final
    {
    public:
        CUBOS_REFLECT;
        
        using is_transparent = void; // Tag to allow heterogeneous lookup

        ~StringTransparentEqual() = default;
        StringTransparentEqual() = default;

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
} // namespace cubos::core::data


