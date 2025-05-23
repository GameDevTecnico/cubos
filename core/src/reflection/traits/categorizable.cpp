#include <string>

#include <cubos/core/reflection/traits/categorizable.hpp>
#include <cubos/core/reflection/type.hpp>
#include <utility>

using cubos::core::reflection::CategorizableTrait;

CUBOS_REFLECT_IMPL(CategorizableTrait)
{
    return Type::create("cubos::core::reflection::CategorizableTrait");
}

CategorizableTrait::CategorizableTrait(std::string category, std::size_t priority)
    : mCategory{std::move(category)}
    , mPriority{priority}
{
}

std::string CategorizableTrait::category() const
{
    return mCategory;
}

std::size_t CategorizableTrait::priority() const
{
    return mPriority;
}
