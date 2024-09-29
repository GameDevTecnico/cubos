#include <cubos/core/al/miniaudio_device.hpp>

using namespace cubos::core::al;

std::shared_ptr<AudioDevice> AudioDevice::create()
{
    return std::make_shared<MiniaudioDevice>();
}

void AudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
    MiniaudioDevice::enumerateDevices(devices);
}
