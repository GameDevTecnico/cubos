#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/data/transparent/string_equal.hpp>

using namespace cubos::core::data;

CUBOS_REFLECT_IMPL(cubos::core::data::StringTransparentEqual)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::core::data::StringTransparentEqual");
}

bool StringTransparentEqual::operator()(std::string_view svhs, std::string_view svvhs) const
{
    return svhs == svvhs;
}

bool StringTransparentEqual::operator()(const std::string& shs, std::string_view svhs) const
{
    return shs == svhs;
}

bool StringTransparentEqual::operator()(std::string_view svhs, const std::string& shs) const
{
    return svhs == shs;
}

bool StringTransparentEqual::operator()(const std::string& shs, const std::string& sshs) const
{
    return shs == sshs;
}
