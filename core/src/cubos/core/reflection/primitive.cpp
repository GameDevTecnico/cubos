#include <cubos/core/reflection/primitive.hpp>
#include <cubos/core/reflection/visitor.hpp>

using cubos::core::reflection::PrimitiveType;

PrimitiveType::Builder PrimitiveType::build(std::string name)
{
    return PrimitiveType::Builder{*new PrimitiveType{std::move(name)}};
}

void PrimitiveType::accept(TypeVisitor& visitor) const
{
    visitor.visit(*this);
}

PrimitiveType::PrimitiveType(std::string name)
    : Type(std::move(name))
{
}