#include <cubos/core/al/miniaudio_context.hpp>

using namespace cubos::core::al;

std::shared_ptr<AudioContext> AudioContext::create()
{
    return std::make_shared<MiniaudioContext>();
}
