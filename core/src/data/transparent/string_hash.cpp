#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/data/transparent/string_hash.hpp>

using namespace cubos::core::data;

CUBOS_REFLECT_IMPL(cubos::core::data::StringTransparentHash)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::core::data::StringTransparentHash");
}

std::size_t StringTransparentHash::operator()(const char* c) const
{
    return std::hash<std::string_view>{}(c);
}

std::size_t StringTransparentHash::operator()(std::string_view sv) const
{
    return std::hash<std::string_view>{}(sv);
}

std::size_t StringTransparentHash::operator()(const std::string& s) const
{
    return std::hash<std::string>{}(s);
}
