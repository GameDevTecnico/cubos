#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

/// [Parent and Son declaration]
#include <cubos/core/reflection/reflect.hpp>

struct GrandParent
{
    CUBOS_REFLECT;
};

struct Parent
{
    CUBOS_REFLECT;
};

struct Son
{
    CUBOS_REFLECT;
};
/// [Parent and Son declaration]

/// [Reflection definition]
#include <cubos/core/reflection/traits/inherits.hpp>

using cubos::core::reflection::InheritsTrait;
using cubos::core::reflection::Type;

CUBOS_REFLECT_IMPL(GrandParent)
{
    return Type::create("GrandParent");
}

CUBOS_REFLECT_IMPL(Parent)
{
    return Type::create("Parent").with(InheritsTrait::from<GrandParent>());
}

CUBOS_REFLECT_IMPL(Son)
{
    return Type::create("Son").with(InheritsTrait::from<Parent>());
}
/// [Reflection definition]

/// [Test function]
template <typename T>
void reflectType()
{
    using cubos::core::reflection::reflect;

    const Type& type = reflect<T>();

    if (type.has<InheritsTrait>() && type.get<InheritsTrait>().inherits<Parent>())
    {
        CUBOS_INFO("{} inherits from Parent", type.name());
    }
    else
    {
        CUBOS_INFO("{} DOES NOT inherit from Parent", type.name());
    }

    if (type.has<InheritsTrait>() && type.get<InheritsTrait>().inherits<GrandParent>())
    {
        CUBOS_INFO("{} inherits from GrandParent", type.name());
    }
    else
    {
        CUBOS_INFO("{} DOES NOT inherit from GrandParent", type.name());
    }
}

int main()
{
    reflectType<Son>();
    reflectType<Parent>();
    reflectType<int>();
    return 0;
}
/// [Test function]
