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

        AudioDevice createDevice(unsigned int listenerCount, const std::string& specifier) override;
        Buffer createBuffer(const void* data, size_t dataSize) override;
        void enumerateDevices(std::vector<std::string>& devices) override;

        static std::string getDefaultDevice();

    private:
        ma_context mContext;
    };
} // namespace cubos::core::al
