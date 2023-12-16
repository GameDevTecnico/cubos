#include <cubos/core/ecs/table/column.hpp>

using cubos::core::ecs::DataTypeId;
using cubos::core::ecs::DenseColumnId;

const DenseColumnId DenseColumnId::Invalid = {.inner = UINT64_MAX};

DenseColumnId DenseColumnId::make(DataTypeId id)
{
    return DenseColumnId{.inner =
                             static_cast<uint64_t>(id.inner) << 32 | static_cast<uint64_t>(DataTypeId::Invalid.inner)};
}

DenseColumnId DenseColumnId::make(DataTypeId id, uint32_t index)
{
    return DenseColumnId{.inner = static_cast<uint64_t>(id.inner) << 32 | static_cast<uint64_t>(index)};
}

DataTypeId DenseColumnId::dataType() const
{
    return DataTypeId{.inner = static_cast<uint32_t>(inner >> 32)};
}
