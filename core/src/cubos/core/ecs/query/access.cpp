#include <cubos/core/ecs/query/access.hpp>

using cubos::core::ecs::QueryAccess;

void QueryAccess::insert(DataTypeId id)
{
    dataTypes.emplace(id.inner);
}

bool QueryAccess::intersects(const QueryAccess& other) const
{
    for (auto id : dataTypes)
    {
        if (other.dataTypes.contains(id))
        {
            return true;
        }
    }

    return false;
}
