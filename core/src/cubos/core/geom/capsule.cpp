#include <cubos/core/geom/capsule.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_IMPL(cubos::core::geom::Capsule)
{
    using namespace core::reflection;

    return Type::create("cubos::core::geom::Capsule")
        .with(ConstructibleTrait::typed<Capsule>().withBasicConstructors().build())
        .with(FieldsTrait().withField("radius", &Capsule::radius).withField("length", &Capsule::length));
}
