#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/input/transparent_equal.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::InputTransparentEqual)
{
    using namespace cubos::core::reflection;
    return Type::create("cubos::engine::InputTransparentEqual");
}

bool InputTransparentEqual::operator()(std::string_view svhs, std::string_view svvhs) const 
{
    return svhs == svvhs;
}

bool InputTransparentEqual::operator()(const std::string& shs, std::string_view svhs) const 
{
    return shs == svhs;
}

bool InputTransparentEqual::operator()(std::string_view svhs, const std::string& shs) const 
{
    return svhs == shs;
}

bool InputTransparentEqual::operator()(const std::string& shs, const std::string& sshs) const 
{
    return shs == sshs;
}
