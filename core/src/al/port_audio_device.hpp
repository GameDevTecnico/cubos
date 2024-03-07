#pragma once

#include <portaudio.h>

#include <cubos/core/al/audio_device.hpp>

/// funcao pa criar uma stream? createStream que recebe callback
/// funcao pa criar uma stream? createStream que recebe buffer

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
        Source stream(PortAudioOutputCallbackFn callback) override;

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

        PortAudioOutputCallbackFn callback;

    private:
        int mOutputDeviceID;
        PaStream* mStream;
    };
} // namespace cubos::core::al
