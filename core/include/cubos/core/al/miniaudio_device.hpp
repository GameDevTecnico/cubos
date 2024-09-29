#pragma once

#include <cstddef>

#include <cubos/core/al/audio_device.hpp>

namespace cubos::core::al
{
    class MiniaudioBuffer : public impl::Buffer
    {
    public:
        ~MiniaudioBuffer() override;
        size_t getLength() override;

        static std::shared_ptr<Buffer> create(const void* data, size_t dataSize)
        {
            return std::shared_ptr<cubos::core::al::MiniaudioBuffer>(new MiniaudioBuffer(data, dataSize));
        }

    protected:
        MiniaudioBuffer(const void* data, size_t dataSize);
    };

    class MiniaudioSource : public impl::Source
    {
    public:
        ~MiniaudioSource() override;

        static std::shared_ptr<Source> create()
        {
            return std::shared_ptr<cubos::core::al::MiniaudioSource>(new MiniaudioSource());
        }

        void setBuffer(cubos::core::al::Buffer buffer) override;
        void setPosition(const glm::vec3& position) override;
        void setVelocity(const glm::vec3& velocity) override;
        void setGain(float gain) override;
        void setPitch(float pitch) override;
        void setLooping(bool looping) override;
        void setRelative(bool relative) override;
        void setMaxDistance(float maxDistance) override;
        void setMinDistance(float minDistance) override;
        void setCone(float innerAngle, float outerAngle, float outerGain) override;
        void setConeDirection(const glm::vec3& direction) override;
        void play() override;

    protected:
        MiniaudioSource();

    private:
        ma_sound mSound;
        ma_engine mEngine;
    };

    /// Audio device implementation using miniaudio.
    class MiniaudioDevice : public AudioDevice
    {
    public:
        MiniaudioDevice();
        ~MiniaudioDevice() override;

        static void enumerateDevices(std::vector<std::string>& devices);
        static std::string getDefaultDevice();
        static Buffer createBuffer(const void* data, size_t dataSize);
        static Source createSource();

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
