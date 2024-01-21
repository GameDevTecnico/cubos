#include <cubos/core/ecs/table/column.hpp>

using cubos::core::ecs::ColumnId;
using cubos::core::ecs::DataTypeId;

const ColumnId ColumnId::Invalid = {.inner = UINT64_MAX};

ColumnId ColumnId::make(DataTypeId id)
{
    return ColumnId{.inner = static_cast<uint64_t>(id.inner) << 32 | static_cast<uint64_t>(DataTypeId::Invalid.inner)};
}

ColumnId ColumnId::make(DataTypeId id, uint32_t index)
{
    return ColumnId{.inner = static_cast<uint64_t>(id.inner) << 32 | static_cast<uint64_t>(index)};
}

DataTypeId ColumnId::dataType() const
{
    return DataTypeId{.inner = static_cast<uint32_t>(inner >> 32)};
}

uint32_t ColumnId::index() const
{
    return static_cast<uint32_t>(inner);
}
