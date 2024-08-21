#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/geom/plane.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::core::geom::Plane)
{
    return cubos::core::ecs::TypeBuilder<Plane>("cubos::core::geom::Plane")
        .withField("normal", &Plane::normal)
        .withField("d", &Plane::d)
        .build();
}
