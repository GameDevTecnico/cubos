#include <cubos/core/ecs/system.hpp>

using namespace cubos::core::ecs;

bool SystemInfo::valid() const
{
    for (auto& rsc : this->resourcesRead)
    {
        if (this->resourcesWritten.contains(rsc) || this->usesWorld)
        {
            return false;
        }
    }

    for (auto& comp : this->componentsRead)
    {
        if (this->componentsWritten.contains(comp) || this->usesWorld)
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

AnySystemWrapper::AnySystemWrapper(SystemInfo&& info) : m_info(std::move(info))
{
    if (!this->m_info.valid())
    {
        CUBOS_CRITICAL("System is invalid - this may happen, if, for example, "
                       "it both reads and writes the same resource");
        abort();
    }
}

const SystemInfo& AnySystemWrapper::info() const
{
    return m_info;
}
