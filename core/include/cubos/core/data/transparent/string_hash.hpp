/// @file
/// @brief Class @ref cubos::core::data::StringTransparentHash.
/// @ingroup core-data-transparent

#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/api.hpp>
#include <string>

namespace cubos::core::data
{
    /// @brief Hash function for std::string and std::string_view, implemented as a class.
    ///
    /// This allows using std::string_view as a key in unordered_map, while still allowing
    /// std::string as a key.
    ///
    /// @ingroup core-data-transparent
    class CUBOS_CORE_API StringTransparentHash final
    {
    public:
        CUBOS_REFLECT;
        
        using is_transparent = void; // Tag to allow heterogeneous lookup

        ~StringTransparentHash() = default;
        StringTransparentHash() = default;

        /// @brief Hash function for const char*.
        /// @param c The string to hash.
        /// @return The hash value.
        std::size_t operator()(const char* c) const;

        /// @brief Hash function for std::string_view.
        /// @param sv The string view to hash.
        /// @return The hash value.
        std::size_t operator()(std::string_view sv) const;
       
        /// @brief Hash function for std::string.
        /// @param s The string to hash.
        /// @return The hash value.
        std::size_t operator()(const std::string& s) const;
    };
} // namespace cubos::core::data


