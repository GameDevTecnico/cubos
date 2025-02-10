/// @file
/// @brief Class @ref cubos::engine::TransparentHash.
/// @ingroup input-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <string>

namespace cubos::engine
{
    /// @brief Hash function for std::string and std::string_view, implemented as a class.
    ///
    /// This allows using std::string_view as a key in unordered_map, while still allowing
    /// std::string as a key.
    ///
    /// @ingroup input-plugin
    class CUBOS_ENGINE_API InputTransparentHash final
    {
    public:
        CUBOS_REFLECT;
        
        using IsTransparent = void; // Tag to allow heterogeneous lookup

        ~InputTransparentHash() = default;
        InputTransparentHash() = default;

        /// @brief Hash function for std::string_view.
        /// @param sv The string view to hash.
        /// @return The hash value.
        std::size_t operator()(std::string_view sv) const;
       
        /// @brief Hash function for std::string.
        /// @param s The string to hash.
        /// @return The hash value.
        std::size_t operator()(const std::string& s) const;

    };
} // namespace cubos::engine


