/// @file
/// @brief Class @ref cubos::core::ecs::Tag.
/// @ingroup core-ecs

#pragma once

#include <string>

#include <cubos/core/api.hpp>

/// @brief Utility used to define tags more easily.
///
/// @code{.cpp}
/// // in file.hpp
/// namespace my
/// {
///     cubos::core::ecs::Tag MyTag;
/// }
///
/// // in file.cpp
/// CUBOS_DEFINE_TAG(my::MyTag);
/// @endcode
///
/// @param tag Tag type.
#define CUBOS_DEFINE_TAG(tag) cubos::core::ecs::Tag tag{#tag};

namespace cubos::core::ecs
{
    /// @brief Represents a tag to be used with the @ref Cubos class.
    class CUBOS_CORE_API Tag
    {
    public:
        /// @brief Constructs.
        /// @param name Name.
        Tag(std::string name);

        /// @brief Returns the tag's name.
        /// @return Name.
        std::string name() const;

        /// @brief Returns an unique name for the tag, which is generated from its name and memory address.
        /// @return Unique identifier.
        std::string id() const;

    private:
        std::string mName;
    };
} // namespace cubos::core::ecs