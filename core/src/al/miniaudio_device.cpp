#define MINIAUDIO_IMPLEMENTATION
#include <cubos/core/al/miniaudio_device.hpp>
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
    MiniaudioSource()
    {
        if (ma_engine_init(nullptr, &mEngine) != MA_SUCCESS)
        {
            CUBOS_FAIL("Failed to initialize miniaudio engine.");
            abort();
        }
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
            abort();
        }

        if (ma_sound_init_from_data_source(&mEngine, &miniaudioBuffer->mDecoder, 0, nullptr, &mSound) != MA_SUCCESS)
        {
            CUBOS_ERROR("Failed to initialize sound from buffer.");
            abort();
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
            abort();
        }
    }

private:
    ma_sound mSound;
    ma_engine mEngine;
};

MiniaudioDevice::MiniaudioDevice()
{
    // Initialize miniaudio context.
    if (ma_context_init(nullptr, 0, nullptr, &mContext) != MA_SUCCESS)
    {
        CUBOS_FAIL("Failed to initialize miniaudio context.");
        abort();
    }

    // Initialize miniaudio engine
    if (ma_engine_init(nullptr, &mEngine) != MA_SUCCESS)
    {
        CUBOS_FAIL("Failed to initialize miniaudio engine.");
        abort();
    }

    // Configure the device.
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32; // Set to ma_format_unknown to use the device's native format.
    deviceConfig.playback.channels = 2;           // Set to 0 to use the device's native channel count.
    deviceConfig.sampleRate = 48000;              // Set to 0 to use the device's native sample rate.

    // Initialize the audio device.
    if (ma_device_init(&mContext, &deviceConfig, &mDevice) != MA_SUCCESS)
    {
        CUBOS_FAIL("Failed to initialize audio device.");
        ma_context_uninit(&mContext);
        abort();
    }

    ma_device_start(&mDevice);
}

MiniaudioDevice::~MiniaudioDevice()
{
    ma_device_uninit(&mDevice);
    ma_context_uninit(&mContext);
}

void MiniaudioDevice::enumerateDevices(std::vector<std::string>& devices)
{
    ma_context context;
    if (ma_context_init(nullptr, 0, nullptr, &context) != MA_SUCCESS)
    {
        CUBOS_ERROR("Failed to initialize audio context.");
        abort();
    }

    ma_device_info* pPlaybackDeviceInfos;
    ma_uint32 playbackDeviceCount;
    if (ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, nullptr, nullptr) != MA_SUCCESS)
    {
        CUBOS_FAIL("Failed to enumerate devices.");
        ma_context_uninit(&context); // Uninitialize context before aborting
        abort();
    }

    for (ma_uint32 i = 0; i < playbackDeviceCount; i++)
    {
        devices.emplace_back(pPlaybackDeviceInfos[i].name);
    }

    ma_context_uninit(&context);
}

std::string MiniaudioDevice::getDefaultDevice()
{
    ma_context context;
    if (ma_context_init(nullptr, 0, nullptr, &context) != MA_SUCCESS)
    {
        CUBOS_FAIL("Failed to initialize audio context.");
        abort();
    }

    std::string defaultDeviceName;
    ma_context_enumerate_devices(
        &context,
        [](ma_context*, ma_device_type deviceType, const ma_device_info* pDeviceInfo, void* pUserData) -> ma_bool32 {
            auto* pDefaultDeviceName = static_cast<std::string*>(pUserData);
            if (deviceType == ma_device_type_playback && pDeviceInfo->isDefault == MA_TRUE)
            {
                *pDefaultDeviceName = pDeviceInfo->name; // Set the default device name
                return MA_FALSE;
            }
            return MA_TRUE;
        },
        &defaultDeviceName); // Pass defaultDeviceName as pUserData

    ma_context_uninit(&context);
    return defaultDeviceName;
}

Buffer MiniaudioDevice::createBuffer(const void* data, size_t dataSize)
{
    return std::make_shared<MiniaudioBuffer>(data, dataSize);
}

Source MiniaudioDevice::createSource()
{
    return std::make_shared<MiniaudioSource>();
}

void MiniaudioDevice::setListenerPosition(const glm::vec3& position, ma_uint32 listenerIndex)
{
    ma_engine_listener_set_position(&mEngine, listenerIndex, position.x, position.y, position.z);
}

void MiniaudioDevice::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up, ma_uint32 listenerIndex)
{
    ma_engine_listener_set_direction(&mEngine, listenerIndex, forward.x, forward.y, forward.z);
    ma_engine_listener_set_world_up(&mEngine, listenerIndex, up.x, up.y, up.z);
}

void MiniaudioDevice::setListenerVelocity(const glm::vec3& velocity, ma_uint32 listenerIndex)
{
    ma_engine_listener_set_velocity(&mEngine, listenerIndex, velocity.x, velocity.y, velocity.z);
}
