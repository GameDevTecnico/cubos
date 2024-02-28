#include "port_audio_device.hpp"

using namespace cubos::core::al;

std::shared_ptr<AudioDevice> AudioDevice::create(int deviceIndex)
{
    return std::make_shared<PortAudioDevice>(deviceIndex);
}

int AudioDevice::deviceCount()
{
    return PortAudioDevice::deviceCount();
}

void AudioDevice::enumerateDevices(std::vector<DeviceInfo>& devices, bool debug)
{
    PortAudioDevice::enumerateDevices(devices, debug);
}

DeviceInfo AudioDevice::deviceInfo(int deviceIndex)
{
    return PortAudioDevice::deviceInfo(deviceIndex);
}

void AudioDevice::printDeviceInformation(int deviceIndex)
{
    return PortAudioDevice::printDeviceInformation(deviceIndex);
}