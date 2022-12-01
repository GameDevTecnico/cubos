#include <cubos/core/al/oal_audio_device.hpp>
#include <cubos/core/log.hpp>

#include <AL/al.h>
#include <AL/alc.h>

using namespace cubos::core::al;

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

    

    ALuint id;
};

OALAudioDevice::OALAudioDevice(std::string specifier)
{
    auto device = alcOpenDevice(specifier.c_str());
    auto context = alcCreateContext(device, nullptr); // May be relevant in the future, passes extra attributes
    alcMakeContextCurrent(context);
}

OALAudioDevice::~OALAudioDevice()
{
    auto context = alcGetCurrentContext();
    auto device = alcGetContextsDevice(context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void OALAudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_FALSE)
    {
        logCritical("Missing extension ALC_ENUMERATION_EXT");
        abort();
    }

    const char* pointer = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

    while (*pointer != '\0')
    {
        std::string s = pointer;
        devices.push_back(s);
        pointer += s.size() + 1;
    }
}

std::string OALAudioDevice::getDefaultDevice()
{
    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_FALSE)
    {
        logCritical("Missing extension ALC_ENUMERATION_EXT");
        abort();
    }

    return alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
}

Buffer OALAudioDevice::createBuffer()
{
    ALuint id;
    alGenBuffers(1, &id);

    return std::make_shared<OALBuffer>(id);
}

Source OALAudioDevice::createSource()
{
}

void OALAudioDevice::setListenerPosition(const glm::vec3& position)
{
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void OALAudioDevice::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up)
{
    float orientation[6] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, orientation);
}

void OALAudioDevice::setListenerVelocity(const glm::vec3& velocity)
{
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}
