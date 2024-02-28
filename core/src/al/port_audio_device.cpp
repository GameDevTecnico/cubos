#include "port_audio_device.hpp"

#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#ifdef WITH_PORTAUDIO
#include "../../core/lib/portaudio/include/portaudio.h"
#endif // WITH_PORTAUDIO

#include <array>

#define UNSUPPORTED()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Unsupported when building without PortAudio");                                                 \
        abort();                                                                                                       \
    } while (0)

using namespace cubos::core::al;

PortAudioDevice::PortAudioDevice(const std::string& specifier)
{
#ifdef WITH_PORTAUDIO
    auto err = Pa_Initialize();
    if (err != paNoError)
    {
        CUBOS_FAIL("PortAudio failed to initialize: {}", Pa_GetErrorText(err));
    }
    (void)specifier;
    // TODO....
#else
    (void)specifier;
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}

PortAudioDevice::~PortAudioDevice()
{
#ifdef WITH_PORTAUDIO
    Pa_Terminate();
#else
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}

int PortAudioDevice::getDeviceCount()
{
#ifdef WITH_PORTAUDIO
    return Pa_GetDeviceCount();
#else
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}

void PortAudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
    devices.clear();
    // TODO: should we store PaDeviceInfos instead of strings?
    for (int i = 0; i < getDeviceCount(); i++)
    {
        devices.push_back(std::string(Pa_GetDeviceInfo(i)->name));
    }
}

Buffer PortAudioDevice::createBuffer()
{
#ifdef WITH_PORTAUDIO
    CUBOS_TODO();
#else
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}

Source PortAudioDevice::createSource()
{
#ifdef WITH_PORTAUDIO
    CUBOS_TODO();
#else
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}

void PortAudioDevice::setListenerPosition(const glm::vec3&)
{
#ifdef WITH_PORTAUDIO
    CUBOS_TODO();
#else
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}

void PortAudioDevice::setListenerOrientation(const glm::vec3&, const glm::vec3&)
{
#ifdef WITH_PORTAUDIO
    CUBOS_TODO();
#else
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}

void PortAudioDevice::setListenerVelocity(const glm::vec3&)
{
#ifdef WITH_PORTAUDIO
    CUBOS_TODO();
#else
    UNSUPPORTED();
#endif // WITH_PORTAUDIO
}