#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_IMPL(uuids::uuid)
{
    using namespace cubos::core::reflection;

    return Type::create("uuids::uuid").with(ConstructibleTrait::typed<uuids::uuid>().withBasicConstructors().build());
}
