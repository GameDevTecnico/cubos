#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

/// [Mask declaration]
#include <cubos/core/reflection/reflect.hpp>

enum class Permissions
{
    None = 0,

    Read = 1,
    Write = 2,
    Execute = 4
};

inline Permissions operator~(Permissions p)
{
    return static_cast<Permissions>(~static_cast<int>(p));
}

inline Permissions operator|(Permissions a, Permissions b)
{
    return static_cast<Permissions>(static_cast<int>(a) | static_cast<int>(b));
}

inline Permissions operator&(Permissions a, Permissions b)
{
    return static_cast<Permissions>(static_cast<int>(a) & static_cast<int>(b));
}
/// [Mask declaration]

/// [Reflection definition]
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::MaskTrait;
using cubos::core::reflection::Type;

template <>
CUBOS_REFLECT_EXTERNAL_IMPL(Permissions)
{
    return Type::create("Permissions")
        .with(MaskTrait{}
                  .withBit<Permissions::Read>("Read")
                  .withBit<Permissions::Write>("Write")
                  .withBit<Permissions::Execute>("Execute"));
}
/// [Reflection definition]

/// [Using the type]
int main()
{
    using cubos::core::reflection::reflect;

    const auto& permissionsType = reflect<Permissions>();
    CUBOS_ASSERT(permissionsType.has<MaskTrait>());

    const auto& mask = permissionsType.get<MaskTrait>();
    CUBOS_ASSERT(mask.contains("Read"));
    CUBOS_ASSERT(mask.contains("Write"));
    CUBOS_ASSERT(mask.contains("Execute"));
    CUBOS_ASSERT(!mask.contains("None"));

    auto p = Permissions::Read | Permissions::Write;
    CUBOS_ASSERT(mask.view(&p).test("Read"));
    CUBOS_ASSERT(mask.view(&p).test("Write"));
    CUBOS_ASSERT(!mask.view(&p).test("Execute"));

    for (const auto& b : mask)
    {
        CUBOS_INFO("Type has bit {}", b.name());
    }

    for (const auto& b : mask.view(&p))
    {
        CUBOS_INFO("Value has bit {} set", b.name());
    }

    return 0;
}
/// [Using the type]

/// [Output]
// Type has bit "Read"
// Type has bit "Write"
// Type has bit "Execute"
// Value has bit "Read" set
// Value has bit "Write" set
/// [Output]
