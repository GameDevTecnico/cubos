#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/bloom/bloom.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Bloom)
{
    return core::ecs::TypeBuilder<Bloom>("cubos::engine::Bloom")
        .withField("iterations", &Bloom::iterations)
        .withField("threshold", &Bloom::threshold)
        .withField("softThreshold", &Bloom::softThreshold)
        .withField("intensity", &Bloom::intensity)
        .withField("luminance", &Bloom::luminance)
        .withField("size", &Bloom::size)
        .build();
}
