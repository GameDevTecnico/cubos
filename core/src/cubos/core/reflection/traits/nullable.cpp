#include <cubos/core/reflection/traits/nullable.hpp>

using cubos::core::reflection::NullableTrait;

NullableTrait::NullableTrait(IsNull isNull, SetToNull setToNull)
    : mIsNull(isNull)
    , mSetToNull(setToNull)
{
}

bool NullableTrait::isNull(const void* instance) const
{
    return mIsNull(instance);
}

void NullableTrait::setToNull(void* instance) const
{
    mSetToNull(instance);
}
