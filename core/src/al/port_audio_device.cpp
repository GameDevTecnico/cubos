#include "port_audio_device.hpp"

#include <cubos/core/log.hpp>
#include <cubos/core/memory/function.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#ifdef WITH_PORTAUDIO
#include <portaudio.h>
#endif // WITH_PORTAUDIO

#define SAMPLE_RATE 44100.0   // How many audio samples to capture every second (44100 Hz is standard)
#define FRAMES_PER_BUFFER 512 // How many audio samples to send to our callback function for each channel
#define NUM_CHANNELS 2        // Number of audio channels to capture

#define UNSUPPORTED()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        CUBOS_CRITICAL("Unsupported when building without PortAudio");                                                 \
        abort();                                                                                                       \
    } while (0)

using namespace cubos::core::al;

static int paCallback(const void*, void* outputBuffer, unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags flags, void* userData)
{
    PortAudioDevice* portAudioDevice = static_cast<PortAudioDevice*>(userData);
    return portAudioDevice->callback(outputBuffer, framesPerBuffer, flags, userData);
}

PortAudioDevice::PortAudioDevice(int deviceIndex)
    : outputDeviceID(deviceIndex)
    , stream(nullptr)
{
#ifdef WITH_PORTAUDIO
    auto err = Pa_Initialize();
    if (err != paNoError)
    {
        CUBOS_FAIL("PortAudio failed to initialize: {}", Pa_GetErrorText(err));
    }

    CUBOS_INFO("PortAudio initialized");

    if (outputDeviceID == -1)
    {
        outputDeviceID = Pa_GetDefaultOutputDevice();
        CUBOS_INFO("PortAudio will use default output device ('{}') :", outputDeviceID);
        printDeviceInformation(outputDeviceID);
    }
#else
    UNSUPPORTED();
#endif
}

PortAudioDevice::~PortAudioDevice()
{
    if (stream)
    {
        // FIXME: double stop?»
        stop();
        Pa_CloseStream(stream);
    }

    Pa_Terminate();
}

bool PortAudioDevice::init(PortAudioOutputCallbackFn callback)
{
    this->callback = std::move(callback);
    CUBOS_INFO("Custom output callback function set");

    PaStreamParameters outputParameters;
    outputParameters.device = outputDeviceID;
    // FIXME: what should be specified by the user?
    outputParameters.channelCount = Pa_GetDeviceInfo(outputDeviceID)->maxOutputChannels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputDeviceID)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    CUBOS_INFO("Creating PortAudio stream..");
    auto err =
        Pa_OpenStream(&stream, nullptr, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, paCallback, this);
    if (err != paNoError)
    {
        CUBOS_ERROR("Could not open PortAudio stream: {}", Pa_GetErrorText(err));
        return false;
    }

    return true;
}

void PortAudioDevice::start()
{
    auto err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        CUBOS_ERROR("PortAudio failed to start the stream: {}", Pa_GetErrorText(err));
    }
}

void PortAudioDevice::stop()
{
    auto err = Pa_StopStream(stream);
    if (err != paNoError)
    {
        CUBOS_ERROR("PortAudio failed to stop the stream: {}", Pa_GetErrorText(err));
    }
}

int PortAudioDevice::deviceCount()
{
    return Pa_GetDeviceCount();
}

void PortAudioDevice::enumerateDevices(std::vector<DeviceInfo>& devices, bool debug)
{
    devices.clear();
    for (int i = 0; i < deviceCount(); i++)
    {
        auto deviceInfo = Pa_GetDeviceInfo(i);
        devices.emplace_back(DeviceInfo{.name = deviceInfo->name,
                                        .maxInputChannels = deviceInfo->maxInputChannels,
                                        .maxOutputChannels = deviceInfo->maxOutputChannels,
                                        .defaultSampleRate = deviceInfo->defaultSampleRate});
        if (debug)
        {
            CUBOS_DEBUG("Device '{}' : '{}'", i, deviceInfo->name);
        }
    }
}

DeviceInfo PortAudioDevice::deviceInfo(int deviceIndex)
{
    for (int i = 0; i < deviceCount(); i++)
    {
        if (i == deviceIndex)
        {
            auto deviceInfo = Pa_GetDeviceInfo(i);
            return DeviceInfo{.name = deviceInfo->name,
                              .maxInputChannels = deviceInfo->maxInputChannels,
                              .maxOutputChannels = deviceInfo->maxOutputChannels,
                              .defaultSampleRate = deviceInfo->defaultSampleRate};
        }
    }
    CUBOS_FAIL("Could not find device");
}

void PortAudioDevice::printDeviceInformation(int deviceIndex)
{
    for (int i = 0; i < deviceCount(); i++)
    {
        if (i == deviceIndex)
        {
            auto deviceInfo = Pa_GetDeviceInfo(i);
            CUBOS_INFO("Device '{}':", deviceIndex);
            CUBOS_INFO("\tname: '{}'", deviceInfo->name);
            CUBOS_INFO("\tmaxInputChannels: '{}'", deviceInfo->maxInputChannels);
            CUBOS_INFO("\tmaxOutputChannels: '{}'", deviceInfo->maxOutputChannels);
            CUBOS_INFO("\tdefaultSampleRate: '{}'", deviceInfo->defaultSampleRate);
            return;
        }
    }
    CUBOS_FAIL("Could not find device");
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
