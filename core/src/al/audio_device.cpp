#include "port_audio_device.hpp"

using namespace cubos::core::al;

std::shared_ptr<AudioDevice> AudioDevice::create(const std::string& specifier)
{
    return std::make_shared<PortAudioDevice>(specifier);
}

int AudioDevice::getDeviceCount()
{
    return PortAudioDevice::getDeviceCount();
}

void AudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
    PortAudioDevice::enumerateDevices(devices);
}
