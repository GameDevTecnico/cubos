#pragma once

#include <cstddef>

#include <miniaudio.h>

#include <cubos/core/al/audio_device.hpp>

namespace cubos::core::al
{
    /// Audio device implementation using miniaudio.
    class MiniaudioDevice : public AudioDevice
    {
    public:
        MiniaudioDevice();
        ~MiniaudioDevice() override;

        static void enumerateDevices(std::vector<std::string>& devices);
        static std::string getDefaultDevice();

        Buffer createBuffer(const void* data, size_t dataSize) override;
        Source createSource() override;
        void setListenerPosition(const glm::vec3& position, ma_uint32 listenerIndex = 0) override;
        void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up,
                                    ma_uint32 listenerIndex = 0) override;
        void setListenerVelocity(const glm::vec3& velocity, ma_uint32 listenerIndex = 0) override;

    private:
        ma_context mContext;
        ma_device mDevice;
        ma_engine mEngine;
    };
} // namespace cubos::core::al
