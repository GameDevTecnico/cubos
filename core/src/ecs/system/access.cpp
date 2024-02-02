#include <cubos/core/ecs/system/access.hpp>

bool cubos::core::ecs::SystemAccess::intersects(const SystemAccess& other) const
{
    if (usesWorld || other.usesWorld)
    {
        return true;
    }

    for (const auto& id : dataTypes)
    {
        if (other.dataTypes.contains(id))
        {
            return true;
        }
    }

    return false;
}
