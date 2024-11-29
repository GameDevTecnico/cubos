#define MINIAUDIO_IMPLEMENTATION
#include <cstring>

#include <cubos/core/al/miniaudio_context.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core::al;

#ifdef CUBOS_CORE_MINIAUDIO
class MiniaudioBuffer : public impl::Buffer
{
public:
    ma_decoder decoder;

    MiniaudioBuffer(const void* srcData, size_t dataSize)
    {
        mData = operator new(dataSize);
        std::memcpy(mData, srcData, dataSize);

        if (ma_decoder_init_memory(mData, dataSize, nullptr, &decoder) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to initialize Decoder from data");
        }
        else
        {
            mValid = true;
        }
    }

    ~MiniaudioBuffer() override
    {
        ma_decoder_uninit(&decoder);
        operator delete(mData);
    }

    float length() override
    {
        ma_uint64 lengthInPCMFrames;
        ma_result result = ma_decoder_get_length_in_pcm_frames(&decoder, &lengthInPCMFrames);

        if (result != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to get the length of audio in PCM frames");
            return 0;
        }

        // Calculate the length in seconds: Length in PCM frames divided by the sample rate.
        return static_cast<float>(lengthInPCMFrames) / static_cast<float>(decoder.outputSampleRate);
    }

    bool isValid() const
    {
        return mValid;
    }

private:
    bool mValid = false;
    void* mData;
};

class MiniaudioListener : public impl::Listener
{
public:
    MiniaudioListener(ma_engine& engine, unsigned int index)
        : mEngine(engine)
        , mIndex(index)
    {
    }

    ~MiniaudioListener() override = default;

    void setPosition(const glm::vec3& position) override
    {
        ma_engine_listener_set_position(&mEngine, mIndex, position.x, position.y, position.z);
    }

    void setOrientation(const glm::vec3& forward, const glm::vec3& up) override
    {
        ma_engine_listener_set_direction(&mEngine, mIndex, forward.x, forward.y, forward.z);
        ma_engine_listener_set_world_up(&mEngine, mIndex, up.x, up.y, up.z);
    }

    void setVelocity(const glm::vec3& velocity) override
    {
        ma_engine_listener_set_velocity(&mEngine, mIndex, velocity.x, velocity.y, velocity.z);
    }

    unsigned int index() const
    {
        return mIndex;
    }

private:
    ma_engine& mEngine;
    unsigned int mIndex;
};

class MiniaudioSource : public impl::Source
{
public:
    MiniaudioSource(ma_engine& engine)
        : mSound({})
        , mEngine(engine)
    {
    }

    ~MiniaudioSource() override
    {
        ma_sound_uninit(&mSound);
    }

    void setBuffer(Buffer buffer) override
    {
        ma_sound_uninit(&mSound);

        // Try to dynamically cast the Buffer to a MiniaudioBuffer.
        auto miniaudioBuffer = std::static_pointer_cast<MiniaudioBuffer>(buffer);

        if (ma_sound_init_from_data_source(&mEngine, &miniaudioBuffer->decoder, 0, nullptr, &mSound) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to initialize sound from buffer");
            return;
        }
    }

    void setPosition(const glm::vec3& position) override
    {
        ma_sound_set_position(&mSound, position.x, position.y, position.z);
    }

    void setVelocity(const glm::vec3& velocity) override
    {
        ma_sound_set_velocity(&mSound, velocity.x, velocity.y, velocity.z);
    }

    void setGain(float gain) override
    {
        ma_sound_set_volume(&mSound, gain);
    }

    void setPitch(float pitch) override
    {
        ma_sound_set_pitch(&mSound, pitch);
    }

    void setLooping(bool looping) override
    {
        ma_sound_set_looping(&mSound, static_cast<ma_bool32>(looping));
    }

    void setRelative(Listener listener) override
    {
        CUBOS_ASSERT(listener != nullptr);

        // Try to dynamically cast the Listener to a MiniaudioListener.
        auto miniaudioListener = std::static_pointer_cast<MiniaudioListener>(listener);

        ma_sound_set_pinned_listener_index(&mSound, miniaudioListener->index());

        ma_sound_set_positioning(&mSound, ma_positioning_relative);
    }

    void setMaxDistance(float maxDistance) override
    {
        ma_sound_set_max_distance(&mSound, maxDistance);
    }

    void setMinDistance(float minDistance) override
    {
        ma_sound_set_min_distance(&mSound, minDistance);
    }

    void setCone(float innerAngle, float outerAngle, float outerGain = 1.0F) override
    {
        ma_sound_set_cone(&mSound, innerAngle, outerAngle, outerGain);
    }

    void setConeDirection(const glm::vec3& direction) override
    {
        ma_sound_set_direction(&mSound, direction.x, direction.y, direction.z);
    }

    void play() override
    {
        if (ma_sound_start(&mSound) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to start sound");
            return;
        }
    }

    void stop() override
    {
        if (ma_sound_stop(&mSound) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to stop sound");
            return;
        }
        ma_sound_seek_to_pcm_frame(&mSound, 0);
    }

    void pause() override
    {
        if (ma_sound_stop(&mSound) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to pause sound");
            return;
        }
    }

private:
    ma_sound mSound;
    ma_engine& mEngine;
};

class MiniaudioDevice : public impl::AudioDevice
{
public:
    MiniaudioDevice(ma_context& context, const std::string& deviceName, ma_uint32 listenerCount)
        : mContext(context)
    {
        if (listenerCount > MA_ENGINE_MAX_LISTENERS)
        {
            CUBOS_FAIL("Maximum number of listeners is 4");
            return;
        }

        ma_device_info* pPlaybackDeviceInfos;
        ma_uint32 playbackDeviceCount;
        ma_result result =
            ma_context_get_devices(&mContext, &pPlaybackDeviceInfos, &playbackDeviceCount, nullptr, nullptr);

        if (result != MA_SUCCESS)
        {
            CUBOS_FAIL("Failed to enumerate audio devices");
            return;
        }

        ma_device_id* deviceId = nullptr;

        if (!deviceName.empty()) // in case a device name is provided
        {
            for (ma_uint32 i = 0; i < playbackDeviceCount; i++)
            {
                if (deviceName == pPlaybackDeviceInfos[i].name)
                {
                    deviceId = &pPlaybackDeviceInfos[i].id;
                    break;
                }
            }

            if (deviceId == nullptr)
            {
                CUBOS_FAIL("Audio device '{}' not found", deviceName);
                return;
            }
        }

        ma_engine_config engineConfig = ma_engine_config_init();

        engineConfig.listenerCount = listenerCount;
        engineConfig.pPlaybackDeviceID = deviceId;

        if (ma_engine_init(&engineConfig, &mEngine) != MA_SUCCESS)
        {
            CUBOS_FAIL("Failed to initialize audio engine");
            return;
        }

        mValid = true;

        mListeners.reserve(listenerCount);
        for (ma_uint32 i = 0; i < listenerCount; ++i)
        {
            mListeners.emplace_back(std::make_shared<MiniaudioListener>(mEngine, i));
        }
    }

    ~MiniaudioDevice() override
    {
        ma_engine_uninit(&mEngine);
        ma_device_uninit(&mDevice);
    }

    Source createSource() override
    {
        return std::make_shared<MiniaudioSource>(mEngine);
    }

    Listener listener(size_t index) override
    {
        if (index >= mListeners.size())
        {
            CUBOS_ERROR("Listener index out of range");
            return nullptr;
        }
        return mListeners[index];
    }

    bool isValid() const
    {
        return mValid;
    }

private:
    ma_context mContext;
    ma_device mDevice;
    ma_engine mEngine;
    std::vector<std::shared_ptr<MiniaudioListener>> mListeners;
    bool mValid = false;
};
#endif // CUBOS_CORE_MINIAUDIO

MiniaudioContext::MiniaudioContext()
{
#ifdef CUBOS_CORE_MINIAUDIO
    if (ma_context_init(nullptr, 0, nullptr, &mContext) != MA_SUCCESS)
    {
        CUBOS_FAIL("Failed to initialize audio context.");
        return;
    }
#endif
}

MiniaudioContext::~MiniaudioContext()
{
#ifdef CUBOS_CORE_MINIAUDIO
    ma_context_uninit(&mContext);
#endif
}

int MiniaudioContext::getMaxListenerCount() const
{
#ifdef CUBOS_CORE_MINIAUDIO
    return MA_ENGINE_MAX_LISTENERS;
#else
    return 0;
#endif
}

std::string MiniaudioContext::getDefaultDevice()
{
#ifdef CUBOS_CORE_MINIAUDIO
    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;

    if (ma_context_get_devices(&mContext, &pPlaybackDeviceInfos, &playbackDeviceCount, nullptr, nullptr) != MA_SUCCESS)
    {
        CUBOS_ERROR("Failed to enumerate audio devices when searching for default");
        return "";
    }

    for (ma_uint32 i = 0; i < playbackDeviceCount; i++)
    {
        if (pPlaybackDeviceInfos[i].isDefault != 0U)
        {
            return pPlaybackDeviceInfos[i].name;
        }
    }

    CUBOS_WARN("No default audio device found");
    return "";
#else
    return "";
#endif
}

void MiniaudioContext::enumerateDevices(std::vector<std::string>& devices)
{
#ifdef CUBOS_CORE_MINIAUDIO
    devices.clear();

    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;

    if (ma_context_get_devices(&mContext, &pPlaybackDeviceInfos, &playbackDeviceCount, nullptr, nullptr) != MA_SUCCESS)
    {
        CUBOS_ERROR("Failed to enumerate audio devices");
        return;
    }

    devices.reserve(playbackDeviceCount);

    for (ma_uint32 i = 0; i < playbackDeviceCount; i++)
    {
        devices.emplace_back(pPlaybackDeviceInfos[i].name);
    }

    if (devices.empty())
    {
        CUBOS_WARN("No audio playback devices found");
    }
#else
    (void)devices;
#endif
}

Buffer MiniaudioContext::createBuffer(const void* data, size_t dataSize)
{
#ifdef CUBOS_CORE_MINIAUDIO
    auto buffer = std::make_shared<MiniaudioBuffer>(data, dataSize);
    if (!buffer->isValid())
    {
        CUBOS_ERROR("Failed to create MiniaudioBuffer");
        return nullptr;
    }

    return buffer;
#else
    (void)data;
    (void)dataSize;
    return nullptr;
#endif
}

AudioDevice MiniaudioContext::createDevice(unsigned int listenerCount, const std::string& specifier)
{
#ifdef CUBOS_CORE_MINIAUDIO
    auto device = std::make_shared<MiniaudioDevice>(mContext, specifier, listenerCount);
    if (!device->isValid())
    {
        CUBOS_ERROR("Failed to create MiniaudioDevice");
        return nullptr;
    }

    return device;
#else
    (void)listenerCount;
    return nullptr;
#endif
}
