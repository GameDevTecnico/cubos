#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/tools/toolbox/toolbox.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Toolbox)
{
    return cubos::core::ecs::TypeBuilder<Toolbox>("cubos::engine::Toolbox").build();
}

bool Toolbox::isOpen(const std::string& toolName)
{
    if (mToolsMap.contains(toolName))
    {
        return mToolsMap[toolName];
    }
    mToolsMap[toolName] = false;
    return false;
}

void Toolbox::open(const std::string& toolName)
{
    mToolsMap[toolName] = true;
}

void Toolbox::close(const std::string& toolName)
{
    mToolsMap[toolName] = false;
}

void Toolbox::toggle(const std::string& toolName)
{
    mToolsMap[toolName] = !mToolsMap[toolName];
}