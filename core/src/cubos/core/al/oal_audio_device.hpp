#ifndef CUBOS_CORE_AL_OAL_AUDIO_DEVICE_HPP
#define CUBOS_CORE_AL_OAL_AUDIO_DEVICE_HPP

#include <cubos/core/al/audio_device.hpp>

namespace cubos::core::al
{
    /// Audio device implementation using OpenAL.
    /// @see AudioDevice.
    class OALAudioDevice : public AudioDevice
    {
    public:
        /// @param specifier Specifies the device to use (empty for default).
        OALAudioDevice(std::string specifier = "");
        virtual ~OALAudioDevice() override;

        /// Enumerates the available devices.
        /// @param devices The vector to fill with the available devices.
        static void enumerateDevices(std::vector<std::string>& devices);

        virtual Buffer createBuffer() override;
        virtual Source createSource() override;
        virtual void setListenerPosition(const glm::vec3& position) override;
        virtual void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) override;
        virtual void setListenerVelocity(const glm::vec3& velocity) override;
    };
} // namespace cubos::core::al

#endif // CUBOS_CORE_AL_OAL_AUDIO_DEVICE_HPP
