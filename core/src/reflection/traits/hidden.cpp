#include <cubos/core/reflection/traits/hidden.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::HiddenTrait;

CUBOS_REFLECT_IMPL(HiddenTrait)
{
    return Type::create("cubos::core::reflection::HiddenTrait");
}

HiddenTrait::HiddenTrait(bool hidden)
    : mHidden{hidden}
{
}

bool HiddenTrait::hidden() const
{
    return mHidden;
}
