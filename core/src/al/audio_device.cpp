#include "oal_audio_device.hpp"

using namespace cubos::core::al;

std::shared_ptr<AudioDevice> AudioDevice::create(const std::string& specifier)
{
    return std::make_shared<OALAudioDevice>(specifier);
}

void AudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
    OALAudioDevice::enumerateDevices(devices);
}
