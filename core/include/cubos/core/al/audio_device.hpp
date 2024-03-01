/// @file
/// @brief Class @ref cubos::core::al::AudioDevice and related types.
/// @ingroup core-al

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/api.hpp>

namespace cubos::core::al
{
    namespace impl
    {
        class Buffer;
        class Source;
    } // namespace impl

    /// @brief Handle to an audio buffer.
    /// @see impl::Buffer - audio buffer interface.
    /// @see AudioDevice::createBuffer()
    /// @ingroup core-al
    using Buffer = std::shared_ptr<impl::Buffer>;

    /// @brief Handle to an audio source.
    /// @see impl::Source - audio source interface.
    /// @see AudioDevice::createSource()
    /// @ingroup core-al
    using Source = std::shared_ptr<impl::Source>;

    /// @brief Possible audio formats.
    enum class Format
    {
        Mono8,
        Mono16,
        Stereo8,
        Stereo16,
    };

    /// @brief Audio device interface used to wrap low-level audio rendering APIs.
    class CUBOS_CORE_API AudioDevice
    {
    public:
        AudioDevice() = default;
        virtual ~AudioDevice() = default;

        /// @brief Forbid copy construction.
        AudioDevice(const AudioDevice&) = delete;

        /// @brief Creates an audio device from a given device @p specifier.
        /// @see enumerateDevices()
        /// @param specifier Device specifier (empty for default).
        /// @return Audio device, or nullptr on failure.
        static std::shared_ptr<AudioDevice> create(const std::string& specifier = "");

        /// @brief Enumerates the available devices.
        /// @param[out] devices Vector to fill with the available devices.
        static void enumerateDevices(std::vector<std::string>& devices);

        /// @brief Creates a new audio buffer
        /// @return Handle of the new buffer.
        virtual Buffer createBuffer() = 0;

        /// @brief Creates a new audio source.
        /// @return Handle of the new source.
        virtual Source createSource() = 0;

        /// @brief Sets the position of the listener.
        /// @param position Position.
        virtual void setListenerPosition(const glm::vec3& position) = 0;

        /// @brief Sets the orientation of the listener.
        /// @param forward Forward direction of the listener.
        /// @param up Up direction of the listener.
        virtual void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) = 0;

        /// @brief Sets the velocity of the listener. Used to implement the doppler effect.
        /// @param velocity Velocity of the listener.
        virtual void setListenerVelocity(const glm::vec3& velocity) = 0;
    };

    /// @brief Namespace to store the abstract types implemented by the audio device implementations.
    namespace impl
    {
        /// @brief Abstract audio buffer.
        class CUBOS_CORE_API Buffer
        {
        public:
            virtual ~Buffer() = default;

            /// @brief Fills the buffer with data.
            /// @param format Audio format of the data.
            /// @param size Size of the buffer in bytes.
            /// @param data Buffer data.
            /// @param frequency Audio frequency.
            virtual void fill(Format format, std::size_t size, const void* data, std::size_t frequency) = 0;

        protected:
            Buffer() = default;
        };

        /// @brief Abstract audio source.
        class CUBOS_CORE_API Source
        {
        public:
            virtual ~Source() = default;

            /// @brief Sets the buffer to be played by the source.
            /// @param buffer Buffer.
            virtual void setBuffer(std::shared_ptr<Buffer> buffer) = 0;

            /// @brief Sets the position of the source, by default, in the world space.
            /// @see setRelative() to change this behavior.
            /// @param position Position.
            virtual void setPosition(const glm::vec3& position) = 0;

            /// @brief Sets the velocity of the source, by default, in the world space.
            /// @param velocity Velocity.
            virtual void setVelocity(const glm::vec3& velocity) = 0;

            /// @brief Sets the gain of the source.
            /// @param gain Gain.
            virtual void setGain(float gain) = 0;

            /// @brief Sets the pitch of the source.
            /// @param pitch Pitch.
            virtual void setPitch(float pitch) = 0;

            /// @brief Sets whether the source plays in a loop.
            /// @param looping Looping flag.
            virtual void setLooping(bool looping) = 0;

            /// @brief Sets whether the source position and velocity is relative to the listener or
            /// not.
            /// @param relative Relative flag.
            virtual void setRelative(bool relative) = 0;

            /// @brief Sets the maximum distance at which the source is audible.
            /// @param maxDistance Maximum distance.
            virtual void setDistance(float maxDistance) = 0;

            /// @brief Sets the cone angle of the source, in degrees. By default, 360.
            /// @param coneAngle Angle, in degrees.
            virtual void setConeAngle(float coneAngle) = 0;

            /// @brief Sets the cone gain of the source.
            /// @todo Find out what this is.
            /// @param coneGain Gain.
            virtual void setConeGain(float coneGain) = 0;

            /// @brief Sets the cone direction of the source.
            /// @param direction Direction.
            virtual void setConeDirection(const glm::vec3& direction) = 0;

            /// @brief Sets the distance under which the volume for the source would normally drop
            /// by half.
            /// @param referenceDistance Distance.
            virtual void setReferenceDistance(float referenceDistance) = 0;

            /// @brief Plays the source.
            virtual void play() = 0;

        protected:
            Source() = default;
        };
    } // namespace impl
} // namespace cubos::core::al
