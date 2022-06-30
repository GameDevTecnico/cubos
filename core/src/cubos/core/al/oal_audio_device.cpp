#include <cubos/core/al/oal_audio_device.hpp>

#include <AL/al.h>

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

    virtual Buffer fill(Format format, size_t size, const void* data, size_t frequency) override
    {
        // TODO
        // call alBufferData
    }

    ALuint id;
};

OALAudioDevice::OALAudioDevice()
{
    /*alcOpenDevice(nullptr);
    alcCreateContext(alcGetContextsDevice(nullptr), nullptr);
    alcMakeContextCurrent(alcGetCurrentContext());*/
}

OALAudioDevice::~OALAudioDevice()
{
    /*alcMakeContextCurrent(nullptr);
    alcDestroyContext(alcGetCurrentContext());
    alcCloseDevice(alcGetContextsDevice(nullptr));*/
}

void OALAudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
    // TODO
}

Buffer OALAudioDevice::createBuffer()
{
    ALuint id;
    // TODO

    return std::make_shared<OALBuffer>(id);
}

Source OALAudioDevice::createSource()
{
    // TODO
}

void OALAudioDevice::setListenerPosition(const glm::vec3& position)
{
    // TODO
}

void OALAudioDevice::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up)
{
    // TODO
}

void OALAudioDevice::setListenerVelocity(const glm::vec3& velocity)
{
    // TODO
}

