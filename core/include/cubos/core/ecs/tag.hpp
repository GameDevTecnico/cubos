/// @file
/// @brief Class @ref cubos::core::ecs::Tag.
/// @ingroup core-ecs

#pragma once
#include <string>

#define CUBOS_DEFINE_TAG(tag) cubos::core::ecs::Tag tag{#tag};

namespace cubos::core::ecs
{
    class Tag
    {
    public:
        Tag(std::string name);

        std::string name() const;
        std::string id() const;

    private:
        std::string mName;
    };
} // namespace cubos::core::ecs