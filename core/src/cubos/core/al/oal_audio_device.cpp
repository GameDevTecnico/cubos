#include <cubos/core/al/oal_audio_device.hpp>
#include <cubos/core/log.hpp>

#ifdef WITH_OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#endif // WITH_OPENAL

#define UNSUPPORTED()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Unsupported when building without OpenAL");                                                    \
        abort();                                                                                                       \
    } while (0)

using namespace cubos::core::al;

#ifdef WITH_OPENAL
class OALBuffer : public impl::Buffer
{
public:
    OALBuffer(ALuint id) : id(id)
    {
    }

    virtual ~OALBuffer() override
    {
        alDeleteBuffers(1, &this->id);
    }

    virtual void fill(Format format, size_t size, const void* data, size_t frequency) override
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

        alBufferData(this->id, alFormat, data, size, frequency);
    }

    ALuint id;
};

class OALSource : public impl::Source
{
public:
    OALSource(ALuint id) : id(id)
    {
    }

    virtual ~OALSource() override
    {
        alDeleteSources(1, &this->id);
    }

    virtual void setBuffer(Buffer buffer) override
    {
        auto oalBuffer = std::dynamic_pointer_cast<OALBuffer>(buffer);
        alSourcei(this->id, AL_BUFFER, oalBuffer->id);
    }

    virtual void setPosition(const glm::vec3& position) override
    {
        alSource3f(this->id, AL_POSITION, position.x, position.y, position.z);
    }

    virtual void setVelocity(const glm::vec3& velocity) override
    {
        alSource3f(this->id, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    virtual void setGain(float gain) override
    {
        alSourcef(this->id, AL_GAIN, gain);
    }

    virtual void setPitch(float pitch) override
    {
        alSourcef(this->id, AL_PITCH, pitch);
    }

    virtual void setLooping(bool looping) override
    {
        alSourcei(this->id, AL_LOOPING, looping);
    }

    virtual void setRelative(bool relative) override
    {
        alSourcei(this->id, AL_SOURCE_RELATIVE, relative);
    }

    virtual void setDistance(float maxDistance) override
    {
        alSourcef(this->id, AL_MAX_DISTANCE, maxDistance);
    }

    virtual void setConeAngle(float coneAngle) override
    {
        alSourcef(this->id, AL_CONE_OUTER_ANGLE, coneAngle);
    }

    virtual void setConeGain(float coneGain) override
    {
        alSourcef(this->id, AL_CONE_INNER_ANGLE, coneGain);
    }

    virtual void setConeDirection(const glm::vec3& direction) override
    {
        alSource3f(this->id, AL_DIRECTION, direction.x, direction.y, direction.z);
    }

    virtual void setReferenceDistance(float referenceDistance) override
    {
        alSourcef(this->id, AL_REFERENCE_DISTANCE, referenceDistance);
    }

    virtual void play() override
    {
        alSourcePlay(this->id);
    }

    ALuint id;
};
#endif // WITH_OPENAL

OALAudioDevice::OALAudioDevice(std::string specifier)
{
#ifdef WITH_OPENAL
    auto device = alcOpenDevice(specifier.c_str());
    auto context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}

OALAudioDevice::~OALAudioDevice()
{
#ifdef WITH_OPENAL
    auto context = alcGetCurrentContext();
    auto device = alcGetContextsDevice(context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}

void OALAudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
#ifdef WITH_OPENAL
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
    UNSUPPORTED();
#endif // WITH_OPENAL
}

std::string OALAudioDevice::getDefaultDevice()
{
#ifdef WITH_OPENAL
    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_FALSE)
    {
        CUBOS_CRITICAL("Missing extension ALC_ENUMERATION_EXT");
        abort();
    }

    return alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}

Buffer OALAudioDevice::createBuffer()
{
#ifdef WITH_OPENAL
    ALuint id;
    alGenBuffers(1, &id);

    return std::make_shared<OALBuffer>(id);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}

Source OALAudioDevice::createSource()
{
#ifdef WITH_OPENAL
    ALuint sources;
    alGenSources(1, &sources);

    return std::make_shared<OALSource>(sources);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}

void OALAudioDevice::setListenerPosition(const glm::vec3& position)
{
#ifdef WITH_OPENAL
    alListener3f(AL_POSITION, position.x, position.y, position.z);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}

void OALAudioDevice::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up)
{
#ifdef WITH_OPENAL
    float orientation[6] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}

void OALAudioDevice::setListenerVelocity(const glm::vec3& velocity)
{
#ifdef WITH_OPENAL
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
#else
    UNSUPPORTED();
#endif // WITH_OPENAL
}
