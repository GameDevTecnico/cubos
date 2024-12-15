#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/audio/source.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::AudioSource)
{
    return core::ecs::TypeBuilder<AudioSource>("cubos::engine::AudioSource")
        .withField("playing", &AudioSource::playing)
        .withField("looping", &AudioSource::looping)
        .withField("gain", &AudioSource::gain)
        .withField("pitch", &AudioSource::pitch)
        .withField("maxDistance", &AudioSource::maxDistance)
        .withField("minDistance", &AudioSource::minDistance)
        .withField("innerConeAngle", &AudioSource::innerConeAngle)
        .withField("outerConeAngle", &AudioSource::outerConeAngle)
        .withField("outerConeGain", &AudioSource::outerConeGain)
        .withField("sound", &AudioSource::sound)
        .build();
}
