#include <algorithm>

#include <cubos/core/ecs/system.hpp>

using namespace cubos::core::ecs;

bool SystemInfo::valid() const
{
    if (std::ranges::any_of(this->resourcesRead,
                            [this](const auto& comp) { return this->resourcesWritten.contains(comp); }))
    {
        return false;
    }

    return !std::ranges::any_of(this->componentsRead,
                                [this](const auto& comp) { return this->componentsWritten.contains(comp); });
}

bool SystemInfo::compatible(const SystemInfo& other) const
{
    if (this->usesWorld || other.usesWorld)
    {
        return false;
    }

    if (std::ranges::any_of(this->resourcesRead,
                            [other](const auto& comp) { return other.resourcesWritten.contains(comp); }))
    {
        return false;
    }

    if (std::ranges::any_of(this->componentsRead,
                            [other](const auto& comp) { return other.componentsWritten.contains(comp); }))
    {
        return false;
    }

    if (std::ranges::any_of(other.resourcesRead,
                            [this](const auto& comp) { return this->resourcesWritten.contains(comp); }))
    {
        return false;
    }

    return !std::ranges::any_of(other.componentsRead,
                                [this](const auto& comp) { return this->componentsWritten.contains(comp); });
}
