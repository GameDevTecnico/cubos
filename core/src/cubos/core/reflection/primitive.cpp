#include <cubos/core/reflection/primitive.hpp>
#include <cubos/core/reflection/visitor.hpp>

using cubos::core::reflection::PrimitiveType;

#define AUTO_IMPL(type, name)                                                                                          \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return PrimitiveType::build(name).defaultConstructible<type>().get();                                          \
    }

AUTO_IMPL(bool, "bool")
AUTO_IMPL(char, "char")
AUTO_IMPL(int8_t, "int8_t")
AUTO_IMPL(int16_t, "int16_t")
AUTO_IMPL(int32_t, "int32_t")
AUTO_IMPL(int64_t, "int64_t")
AUTO_IMPL(uint8_t, "uint8_t")
AUTO_IMPL(uint16_t, "uint16_t")
AUTO_IMPL(uint32_t, "uint32_t")
AUTO_IMPL(uint64_t, "uint64_t")
AUTO_IMPL(float, "float")
AUTO_IMPL(double, "double")
AUTO_IMPL(std::string, "std::string")

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