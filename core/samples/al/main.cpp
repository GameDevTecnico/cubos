#include <iostream>

#include <stdio.h>

#include <cubos/core/al/audio_device.hpp>

int main()
{
    auto audio = cubos::core::al::AudioDevice::create();

    std::vector<cubos::core::al::DeviceInfo> devices;
    audio->enumerateDevices(devices, true);

    audio->init([](void* outputBuffer, unsigned long framesPerBuffer, unsigned long, void*) -> int {
        float* out = static_cast<float*>(outputBuffer);
        for (unsigned int i = 0; i < framesPerBuffer; i++)
        {
            *out++ = 0.5f * static_cast<float>(std::sin(2.0 * 3.141592653589793 * 440.0 * i / 44100.0));
        }
        return 0;
    });
    audio->start();

    std::cout << "press enter to stop the sound";
    std::cin.get();

    audio->stop();
}