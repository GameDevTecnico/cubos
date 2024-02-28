#pragma once

#include <portaudio.h>

#include <cubos/core/al/audio_device.hpp>

namespace cubos::core::al
{
    /// Audio device implementation using PortAudio.
    /// @see AudioDevice.
    class PortAudioDevice : public AudioDevice
    {
    public:
        /// @brief Initializes the PortAudio API.
        /// @param deviceIndex Specifies the device id to use (empty for default).
        PortAudioDevice(int deviceIndex = -1);
        ~PortAudioDevice() override;

        /// @brief Creates the stream.
        /// @param callback Supplied function that is responsible for processing and filling input and output buffers.
        /// @return Whether the stream was successfully created.
        bool init(PortAudioOutputCallbackFn callback) override;

        /// @brief Starts the stream.
        void start() override;

        /// @brief Stops the stream.
        void stop() override;

        /// @brief Retrieve the number of available devices. The number of available devices may be zero.
        static int deviceCount();

        /// Enumerates the available devices.
        /// @param[out] devices Vector to fill with the available devices.
        /// @param debug If true, will print the devices using CUBOS_DEBUG.
        static void enumerateDevices(std::vector<DeviceInfo>& devices, bool debug = false);

        /// @brief Retrieve the device information by its index.
        static DeviceInfo deviceInfo(int deviceIndex);

        /// @brief Prints device information by its index.
        static void printDeviceInformation(int deviceIndex);

        Buffer createBuffer() override;
        Source createSource() override;
        void setListenerPosition(const glm::vec3& position) override;
        void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) override;
        void setListenerVelocity(const glm::vec3& velocity) override;

        PortAudioOutputCallbackFn callback;

    private:
        int outputDeviceID;
        PaStream* stream;
    };
} // namespace cubos::core::al
