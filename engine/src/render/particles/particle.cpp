#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/particles/particle.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Particle)
{
    return core::ecs::TypeBuilder<Particle>("cubos::engine::Particle")
        .withField("position", &Particle::position)
        .withField("velocity", &Particle::velocity)
        .withField("lifetime", &Particle::lifetime)
        .withField("age", &Particle::age)
        .build();
}