#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/transparent_hash.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputTransparentHash)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputTransparentHash");
}

std::size_t InputTransparentHash::operator()(std::string_view sv) const
{
    return std::hash<std::string_view>{}(sv);
}

std::size_t InputTransparentHash::operator()(const std::string& s) const
{
    return std::hash<std::string>{}(s);
}
