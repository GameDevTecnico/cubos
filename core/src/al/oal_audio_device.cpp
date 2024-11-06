#include "oal_audio_device.hpp"

#include <cubos/core/tel/logging.hpp>

#ifdef CUBOS_CORE_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#endif // CUBOS_CORE_OPENAL

#include <array>

#define UNSUPPORTED()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Unsupported when building without OpenAL");                                                    \
        abort();                                                                                                       \
    } while (0)

using namespace cubos::core::al;

#ifdef CUBOS_CORE_OPENAL
class OALBuffer : public impl::Buffer
{
public:
    OALBuffer(ALuint id)
        : id(id)
    {
    }

    ~OALBuffer() override
    {
        alDeleteBuffers(1, &this->id);
    }

    void fill(Format format, std::size_t size, const void* data, std::size_t frequency) override
    {
        ALenum alFormat = 0;

        switch (format)
        {
        case Format::Mono8:
            alFormat = AL_FORMAT_MONO8;
            break;

        case Format::Mono16:
            alFormat = AL_FORMAT_MONO16;
            break;

        case Format::Stereo8:
            alFormat = AL_FORMAT_STEREO8;
            break;

        case Format::Stereo16:
            alFormat = AL_FORMAT_STEREO16;
            break;
        }

        alBufferData(this->id, alFormat, data, static_cast<ALsizei>(size), static_cast<ALsizei>(frequency));
    }

    ALuint id;
};

class OALSource : public impl::Source
{
public:
    OALSource(ALuint id)
        : id(id)
    {
    }

    ~OALSource() override
    {
        alDeleteSources(1, &this->id);
    }

    void setBuffer(Buffer buffer) override
    {
        auto oalBuffer = std::dynamic_pointer_cast<OALBuffer>(buffer);
        alSourcei(this->id, AL_BUFFER, static_cast<ALint>(oalBuffer->id));
    }

    void setPosition(const glm::vec3& position) override
    {
        alSource3f(this->id, AL_POSITION, position.x, position.y, position.z);
    }

    void setVelocity(const glm::vec3& velocity) override
    {
        alSource3f(this->id, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    void setGain(float gain) override
    {
        alSourcef(this->id, AL_GAIN, gain);
    }

    void setPitch(float pitch) override
    {
        alSourcef(this->id, AL_PITCH, pitch);
    }

    void setLooping(bool looping) override
    {
        alSourcei(this->id, AL_LOOPING, static_cast<ALint>(looping));
    }

    void setRelative(bool relative) override
    {
        alSourcei(this->id, AL_SOURCE_RELATIVE, static_cast<ALint>(relative));
    }

    void setDistance(float maxDistance) override
    {
        alSourcef(this->id, AL_MAX_DISTANCE, maxDistance);
    }

    void setConeAngle(float coneAngle) override
    {
        alSourcef(this->id, AL_CONE_OUTER_ANGLE, coneAngle);
    }

    void setConeGain(float coneGain) override
    {
        alSourcef(this->id, AL_CONE_INNER_ANGLE, coneGain);
    }

    void setConeDirection(const glm::vec3& direction) override
    {
        alSource3f(this->id, AL_DIRECTION, direction.x, direction.y, direction.z);
    }

    void setReferenceDistance(float referenceDistance) override
    {
        alSourcef(this->id, AL_REFERENCE_DISTANCE, referenceDistance);
    }

    void play() override
    {
        alSourcePlay(this->id);
    }

    ALuint id;
};
#endif // CUBOS_CORE_OPENAL

OALAudioDevice::OALAudioDevice(const std::string& specifier)
{
#ifdef CUBOS_CORE_OPENAL
    auto* device = alcOpenDevice(specifier.c_str());
    auto* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
#else
    (void)specifier;
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}

OALAudioDevice::~OALAudioDevice() // NOLINT(modernize-use-equals-default)
{
#ifdef CUBOS_CORE_OPENAL
    auto* context = alcGetCurrentContext();
    auto* device = alcGetContextsDevice(context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
#endif // CUBOS_CORE_OPENAL
}

void OALAudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
#ifdef CUBOS_CORE_OPENAL
    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_FALSE)
    {
        CUBOS_CRITICAL("Missing extension ALC_ENUMERATION_EXT");
        abort();
    }

    const char* pointer = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

    while (*pointer != '\0')
    {
        std::string s = pointer;
        devices.push_back(s);
        pointer += s.size() + 1;
    }
#else
    (void)devices;
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}

std::string OALAudioDevice::getDefaultDevice()
{
#ifdef CUBOS_CORE_OPENAL
    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_FALSE)
    {
        CUBOS_CRITICAL("Missing extension ALC_ENUMERATION_EXT");
        abort();
    }

    return alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
#else
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}

Buffer OALAudioDevice::createBuffer()
{
#ifdef CUBOS_CORE_OPENAL
    ALuint id;
    alGenBuffers(1, &id);

    return std::make_shared<OALBuffer>(id);
#else
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}

Source OALAudioDevice::createSource()
{
#ifdef CUBOS_CORE_OPENAL
    ALuint sources;
    alGenSources(1, &sources);

    return std::make_shared<OALSource>(sources);
#else
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}

void OALAudioDevice::setListenerPosition(const glm::vec3& position)
{
#ifdef CUBOS_CORE_OPENAL
    alListener3f(AL_POSITION, position.x, position.y, position.z);
#else
    (void)position;
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}

void OALAudioDevice::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up)
{
#ifdef CUBOS_CORE_OPENAL
    float orientation[6] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
#else
    (void)forward;
    (void)up;
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}

void OALAudioDevice::setListenerVelocity(const glm::vec3& velocity)
{
#ifdef CUBOS_CORE_OPENAL
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
#else
    (void)velocity;
    UNSUPPORTED();
#endif // CUBOS_CORE_OPENAL
}
