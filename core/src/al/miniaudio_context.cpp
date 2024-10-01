#define MINIAUDIO_IMPLEMENTATION
#include <cubos/core/al/miniaudio_context.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace cubos::core::al;

class MiniaudioBuffer : public impl::Buffer
{
public:
    ma_decoder mDecoder;
    MiniaudioBuffer(const void* data, size_t dataSize)
    {
        if (ma_decoder_init_memory(data, dataSize, nullptr, &mDecoder) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to initialize Decoder from data");
        }
    }

    ~MiniaudioBuffer() override
    {
        ma_decoder_uninit(&mDecoder);
    }

    size_t getLength() override
    {
        ma_uint64 lengthInPCMFrames;
        ma_result result = ma_decoder_get_length_in_pcm_frames(&mDecoder, &lengthInPCMFrames);

        if (result != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to get the length of audio in PCM frames.");
            return 0;
        }

        // Calculate the length in seconds: Length in PCM frames divided by the sample rate.
        return static_cast<size_t>(lengthInPCMFrames) / mDecoder.outputSampleRate;
    }
};

class MiniaudioSource : public impl::Source
{
public:
    MiniaudioSource(ma_engine& engine)
        : mEngine(engine)
    {
    }

    ~MiniaudioSource() override
    {
        ma_sound_uninit(&mSound);
        ma_engine_uninit(&mEngine);
    }

    void setBuffer(Buffer buffer) override
    {
        // Try to dynamically cast the Buffer to a MiniaudioBuffer.
        auto miniaudioBuffer = std::dynamic_pointer_cast<MiniaudioBuffer>(buffer);

        if (miniaudioBuffer == nullptr)
        {
            CUBOS_FAIL("Buffer is not of type MiniaudioBuffer.");
            return;
        }

        if (ma_sound_init_from_data_source(&mEngine, &miniaudioBuffer->mDecoder, 0, nullptr, &mSound) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to initialize sound from buffer.");
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

    void setRelative(bool relative) override
    {
        relative ? ma_sound_set_positioning(&mSound, ma_positioning_relative)
                 : ma_sound_set_positioning(&mSound, ma_positioning_absolute);
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
            CUBOS_ERROR("Failed to start sound.");
            return;
        }
    }

private:
    ma_sound mSound;
    ma_engine mEngine;
};

class MiniaudioDevice : public cubos::core::al::AudioDevice
{
public:
    MiniaudioDevice(ma_context& context, const std::string& deviceName)
        : mContext(context)
    {
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

        ma_engine_config engineConfig = ma_engine_config_init();
        engineConfig.pPlaybackDeviceID = deviceId; // Use the found device ID

        if (ma_engine_init(&engineConfig, &mEngine) != MA_SUCCESS)
        {
            CUBOS_FAIL("Failed to initialize audio engine");
            return;
        }
    }

    ~MiniaudioDevice() override
    {
        ma_device_uninit(&mDevice);
    }

    Buffer createBuffer(const void* data, size_t dataSize) override
    {
        return std::make_shared<MiniaudioBuffer>(data, dataSize);
    }

    Source createSource() override
    {
        return std::make_shared<MiniaudioSource>(mEngine);
    }

    void setListenerPosition(const glm::vec3& position, ma_uint32 listenerIndex) override
    {
        ma_engine_listener_set_position(&mEngine, listenerIndex, position.x, position.y, position.z);
    }

    void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up, ma_uint32 listenerIndex) override
    {
        ma_engine_listener_set_direction(&mEngine, listenerIndex, forward.x, forward.y, forward.z);
        ma_engine_listener_set_world_up(&mEngine, listenerIndex, up.x, up.y, up.z);
    }

    void setListenerVelocity(const glm::vec3& velocity, ma_uint32 listenerIndex) override
    {
        ma_engine_listener_set_velocity(&mEngine, listenerIndex, velocity.x, velocity.y, velocity.z);
    }

private:
    ma_context mContext;
    ma_device mDevice;
    ma_engine mEngine;
};

MiniaudioContext::MiniaudioContext()
{
    if (ma_context_init(nullptr, 0, nullptr, &mContext) != MA_SUCCESS)
    {
        CUBOS_FAIL("Failed to initialize audio context.");
        return;
    }
}

MiniaudioContext::~MiniaudioContext()
{
    ma_context_uninit(&mContext);
}

std::string MiniaudioContext::getDefaultDevice()
{
    // Create a temporary context
    ma_context context;
    if (ma_context_init(nullptr, 0, nullptr, &context) != MA_SUCCESS)
    {
        CUBOS_ERROR("Failed to initialize audio context for default device lookup");
        return "";
    }

    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;

    if (ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, nullptr, nullptr) != MA_SUCCESS)
    {
        CUBOS_ERROR("Failed to enumerate audio devices when searching for default");
        return "";
    }

    ma_context_uninit(&context);

    for (ma_uint32 i = 0; i < playbackDeviceCount; i++)
    {
        if (pPlaybackDeviceInfos[i].isDefault)
        {
            return pPlaybackDeviceInfos[i].name;
        }
    }

    CUBOS_WARN("No default audio device found");
    return "";
}

void MiniaudioContext::enumerateDevices(std::vector<std::string>& devices)
{
    devices.clear();

    // Create a temporary context
    ma_context context;

    if (ma_context_init(nullptr, 0, nullptr, &context) != MA_SUCCESS)
    {
        CUBOS_ERROR("Failed to initialize audio context for enumeration");
        return;
    }

    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;

    if (ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, nullptr, nullptr) != MA_SUCCESS)
    {
        CUBOS_ERROR("Failed to enumerate audio devices.");
        return;
    }

    ma_context_uninit(&context);

    devices.reserve(playbackDeviceCount);

    for (ma_uint32 i = 0; i < playbackDeviceCount; i++)
    {
        devices.emplace_back(pPlaybackDeviceInfos[i].name);
    }

    if (devices.empty())
    {
        CUBOS_WARN("No audio playback devices found");
    }
}

std::shared_ptr<AudioDevice> MiniaudioContext::createDevice(const std::string& specifier)
{
    return std::make_shared<MiniaudioDevice>(mContext, specifier);
}
