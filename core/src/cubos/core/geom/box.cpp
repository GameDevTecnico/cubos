#include <cubos/core/geom/box.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_IMPL(cubos::core::geom::Box)
{
    using namespace core::reflection;

    return Type::create("cubos::core::geom::Box")
        .with(ConstructibleTrait::typed<Box>().withBasicConstructors().build())
        .with(FieldsTrait().withField("halfSize", &Box::halfSize));
}
