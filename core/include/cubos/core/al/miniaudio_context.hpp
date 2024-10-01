#pragma once

#include <cstddef>

#include <miniaudio.h>

#include <cubos/core/al/audio_context.hpp>

namespace cubos::core::al
{
    /// Audio device implementation using miniaudio.
    class MiniaudioContext : public AudioContext
    {
    public:
        MiniaudioContext();
        ~MiniaudioContext() override;

        std::shared_ptr<AudioDevice> createDevice(const std::string& specifier) override;

        static void enumerateDevices(std::vector<std::string>& devices);
        static std::string getDefaultDevice();

    private:
        ma_context mContext;
    };
} // namespace cubos::core::al
