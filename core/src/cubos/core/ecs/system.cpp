#include <cubos/core/ecs/system.hpp>

using namespace cubos::core::ecs;

bool SystemInfo::valid() const
{
    for (auto& rsc : this->resourcesRead)
    {
        if (this->resourcesWritten.contains(rsc))
        {
            return false;
        }
    }

    for (auto& comp : this->componentsRead)
    {
        if (this->componentsWritten.contains(comp))
        {
            return false;
        }
    }

    return true;
}

bool SystemInfo::compatible(const SystemInfo& other) const
{
    if (this->usesDebug || other.usesDebug)
    {
        return false;
    }

    for (auto& rsc : this->resourcesRead)
    {
        if (other.resourcesWritten.contains(rsc))
        {
            return false;
        }
    }

    for (auto& comp : this->componentsRead)
    {
        if (other.componentsWritten.contains(comp))
        {
            return false;
        }
    }

    for (auto& rsc : other.resourcesRead)
    {
        if (this->resourcesWritten.contains(rsc))
        {
            return false;
        }
    }

    for (auto& comp : other.componentsRead)
    {
        if (this->componentsWritten.contains(comp))
        {
            return false;
        }
    }

    return true;
}
