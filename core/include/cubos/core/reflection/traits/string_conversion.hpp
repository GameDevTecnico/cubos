/// @file
/// @brief Class @ref cubos::core::reflection::StringConversionTrait.
/// @ingroup core-reflection

#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::reflection
{
    /// @brief Stores functions for converting a type to and from a string.
    /// @see See @ref examples-core-reflection-traits-string-conversion for an example of using this trait.
    /// @ingroup core-reflection
    class CUBOS_CORE_API StringConversionTrait
    {
    public:
        CUBOS_REFLECT;

        /// @brief Function pointer to convert an instance of the type into a string.
        using Into = std::string (*)(const void* instance);

        /// @brief Function pointer to convert a string into an instance of the type.
        using From = bool (*)(void* instance, const std::string& string);

        /// @brief Constructs.
        /// @param into Into.
        /// @param from From.
        StringConversionTrait(Into into, From from);

        /// @brief Converts an instance of the type into a string.
        /// @param instance Instance.
        /// @return String.
        std::string into(const void* instance) const;

        /// @brief Converts a string to an instance of the type.
        /// @param instance Instance.
        /// @param string String.
        /// @return Whether the string was valid.
        bool from(void* instance, const std::string& string) const;

    private:
        Into mInto;
        From mFrom;
    };
} // namespace cubos::core::reflection
