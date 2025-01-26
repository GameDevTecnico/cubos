#include <cubos/core/geom/frustum.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_IMPL(cubos::core::geom::Frustum)
{
    using namespace core::reflection;

    return Type::create("cubos::core::geom::Frustum")
        .with(ConstructibleTrait::typed<Frustum>().withBasicConstructors().build())
        .with(FieldsTrait()
                  .withField("top", &Frustum::top)
                  .withField("right", &Frustum::right)
                  .withField("bottom", &Frustum::bottom)
                  .withField("left", &Frustum::left)
                  .withField("near", &Frustum::near)
                  .withField("far", &Frustum::far));
}
