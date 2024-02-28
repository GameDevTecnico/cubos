#include <stdio.h>

#include <cubos/core/al/audio_device.hpp>

int main()
{
    auto audio = cubos::core::al::AudioDevice::create();
    std::vector<std::string> devices;
    audio->enumerateDevices(devices);
    printf("All my sound devices:");
    for (const auto& d : devices)
    {
        printf("- %s\n", d.c_str());
    }
}