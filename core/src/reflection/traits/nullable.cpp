#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::NullableTrait;

CUBOS_REFLECT_IMPL(NullableTrait)
{
    return Type::create("cubos::core::ecs::NullableTrait");
}

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
