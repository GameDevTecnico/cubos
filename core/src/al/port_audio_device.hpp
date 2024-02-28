#pragma once

#include <cubos/core/al/audio_device.hpp>

namespace cubos::core::al
{
    /// Audio device implementation using PortAudio.
    /// @see AudioDevice.
    class PortAudioDevice : public AudioDevice
    {
    public:
        /// @param specifier Specifies the device to use (empty for default).
        PortAudioDevice(const std::string& specifier = "");
        ~PortAudioDevice() override;

        /// @brief Retrieve the number of available devices. The number of available devices may be zero.
        static int getDeviceCount();

        /// Enumerates the available devices.
        /// @param devices The vector to fill with the available devices.
        static void enumerateDevices(std::vector<std::string>& devices);

        static std::string getDefaultDevice();

        Buffer createBuffer() override;
        Source createSource() override;
        void setListenerPosition(const glm::vec3& position) override;
        void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) override;
        void setListenerVelocity(const glm::vec3& velocity) override;
    };
} // namespace cubos::core::al
