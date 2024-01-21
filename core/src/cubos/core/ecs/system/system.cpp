#include <cubos/core/ecs/system/system.hpp>

using namespace cubos::core::ecs;

bool SystemInfo::valid() const
{
    if (this->usesWorld)
    {
        return !this->usesCommands && this->resourcesRead.empty() && this->resourcesWritten.empty();
    }

    for (const auto& rsc : this->resourcesRead)
    {
        if (this->resourcesWritten.contains(rsc))
        {
            return false;
        }
    }

    return true;
}

bool SystemInfo::compatible(const SystemInfo& other) const
{
    if (this->usesWorld || other.usesWorld)
    {
        return false;
    }

    for (const auto& rsc : this->resourcesRead)
    {
        if (other.resourcesWritten.contains(rsc))
        {
            return false;
        }
    }

    for (const auto& rsc : this->resourcesWritten)
    {
        if (other.resourcesRead.contains(rsc) || other.resourcesWritten.contains(rsc))
        {
            return false;
        }
    }

    return true;
}
