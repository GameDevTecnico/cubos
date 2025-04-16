#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::NullableTrait;

CUBOS_REFLECT_IMPL(NullableTrait)
{
    return Type::create("cubos::core::reflection::NullableTrait");
}

NullableTrait::NullableTrait(IsNull isNull, SetToNull setToNull)
    : mIsNull(std::move(isNull))
    , mSetToNull(std::move(setToNull))
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
