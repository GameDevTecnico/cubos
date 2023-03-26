#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace cubos::core::al
{
    namespace impl
    {
        class Buffer;
        class Source;
    } // namespace impl

    using Buffer = std::shared_ptr<impl::Buffer>;
    using Source = std::shared_ptr<impl::Source>;

    /// Possible audio formats.
    enum class Format
    {
        Mono8,
        Mono16,
        Stereo8,
        Stereo16,
    };

    /// Abstract audio device type, used to wrap low-level audio rendering APIs such as OpenAL.
    /// @see al::OALRenderDevice.
    class AudioDevice
    {
    public:
        AudioDevice() = default;
        virtual ~AudioDevice() = default;
        AudioDevice(const AudioDevice&) = delete;

        /// Creates an audio device from a given device specifier.
        /// @param specifier The device specifier (empty for default).
        /// @returns The audio device.
        static std::shared_ptr<AudioDevice> create(const std::string& specifier = "");

        /// Enumerates the available devices.
        /// @param devices The vector to fill with the available devices.
        static void enumerateDevices(std::vector<std::string>& devices);

        /// Creates a new audio buffer
        /// @return The handle of the new buffer.
        virtual Buffer createBuffer() = 0;

        /// Creates a new audio source.
        /// @return The handle of the new source.
        virtual Source createSource() = 0;

        /// Sets the position of the listener.
        /// @param position The position of the listener.
        virtual void setListenerPosition(const glm::vec3& position) = 0;

        /// Sets the orientation of the listener.
        /// @param forward The forward direction of the listener.
        /// @param up The up direction of the listener.
        virtual void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) = 0;

        /// @param velocity The velocity of the listener.
        /// @param velocity The velocity of the listener.
        virtual void setListenerVelocity(const glm::vec3& velocity) = 0;
    };

    /// Abstract al types are defined inside this namespace, they should be used (derived) only in audio device
    /// implementations.
    namespace impl
    {
        /// Abstract audio buffer, should not be used directly.
        class Buffer
        {
        public:
            virtual ~Buffer() = default;

            /// Fills the buffer with data.
            /// @param format The audio format.
            /// @param size The size of the buffer in bytes.
            /// @param data The buffer data.
            /// @param frequency The audio frequency.
            virtual void fill(Format format, std::size_t size, const void* data, std::size_t frequency) = 0;

        protected:
            Buffer() = default;
        };

        /// Abstract audio source, should not be used directly.
        class Source
        {
        public:
            virtual ~Source() = default;

            /// Sets the buffer to be played.
            /// @param buffer The buffer to be played.
            virtual void setBuffer(std::shared_ptr<Buffer> buffer) = 0;

            /// Sets the position of the source.
            /// @param position The position of the source.
            virtual void setPosition(const glm::vec3& position) = 0;

            /// Sets the velocity of the source.
            /// @param velocity The velocity of the source.
            virtual void setVelocity(const glm::vec3& velocity) = 0;

            /// Sets the gain of the source.
            /// @param gain The gain of the source.
            virtual void setGain(float gain) = 0;

            /// Sets the pitch of the source.
            /// @param pitch The pitch of the source.
            virtual void setPitch(float pitch) = 0;

            /// Sets the looping of the source.
            /// @param looping The looping of the source.
            virtual void setLooping(bool looping) = 0;

            /// Sets the relative flag of the source.
            /// @param relative The relative flag of the source.
            virtual void setRelative(bool relative) = 0;

            /// Sets the min and max distance of the source.
            /// @param minDistance The min distance of the source.
            /// @param maxDistance The max distance of the source.
            virtual void setDistance(float maxDistance) = 0;

            /// Sets the cone angle of the source.
            /// @param coneAngle The cone angle of the source.
            virtual void setConeAngle(float coneAngle) = 0;

            /// Sets the cone gain of the source.
            /// @param coneGain The cone gain of the source.
            virtual void setConeGain(float coneGain) = 0;

            /// Sets the cone direction of the source.
            /// @param direction The cone direction of the source.
            virtual void setConeDirection(const glm::vec3& direction) = 0;

            /// Sets the reference distance of the source.
            /// @param referenceDistance The reference distance of the source.
            virtual void setReferenceDistance(float referenceDistance) = 0;

            /// Plays the source.
            virtual void play() = 0;

        protected:
            Source() = default;
        };
    } // namespace impl
} // namespace cubos::core::al
