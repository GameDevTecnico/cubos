#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#define AUTO_IMPL(type, name)                                                                                          \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(name).with(ConstructibleTrait::typed<type>().withBasicConstructors().build());             \
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
