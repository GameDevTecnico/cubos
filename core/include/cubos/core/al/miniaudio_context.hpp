#pragma once

#include <cstddef>

#ifdef CUBOS_CORE_MINIAUDIO
#include <miniaudio.h>
#endif

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
        int getMaxListenerCount() const override;
        std::string getDefaultDevice() override;

    private:
#ifdef CUBOS_CORE_MINIAUDIO
        ma_context mContext;
#endif
    };
} // namespace cubos::core::al
