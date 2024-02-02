#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#define AUTO_IMPL(type)                                                                                                \
    CUBOS_REFLECT_EXTERNAL_IMPL(type)                                                                                  \
    {                                                                                                                  \
        return Type::create(#type).with(ConstructibleTrait::typed<type>().withBasicConstructors().build());            \
    }

AUTO_IMPL(bool)

AUTO_IMPL(char)

AUTO_IMPL(signed char)
AUTO_IMPL(short)
AUTO_IMPL(int)
AUTO_IMPL(long)
AUTO_IMPL(long long)

AUTO_IMPL(unsigned char)
AUTO_IMPL(unsigned short)
AUTO_IMPL(unsigned int)
AUTO_IMPL(unsigned long)
AUTO_IMPL(unsigned long long)

AUTO_IMPL(float)
AUTO_IMPL(double)
