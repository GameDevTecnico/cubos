#include <cstdint>

#include <cubos/core/ecs/system/tag.hpp>

using cubos::core::ecs::Tag;

Tag::Tag(std::string name)
{
    mName = std::move(name);
}

std::string Tag::name() const
{
    return mName;
}

std::string Tag::id() const
{
    return mName + "(" + std::to_string(reinterpret_cast<uintptr_t>(this)) + ")";
}