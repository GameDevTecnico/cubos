#include <cubos/core/ecs/tag.hpp>

using namespace cubos::core::ecs;

Tag::Tag(std::string name)
{
    mName = name;
}

std::string Tag::name() const
{
    return mName;
}

std::string Tag::id() const
{
    return mName + "(" + std::to_string(reinterpret_cast<uintptr_t>(this)) + ")";
}