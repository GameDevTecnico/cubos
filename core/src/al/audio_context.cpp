#include <cubos/core/al/miniaudio_context.hpp>
#include <cubos/core/tel/logging.hpp>

using namespace cubos::core::al;

std::shared_ptr<AudioContext> AudioContext::create()
{
#ifdef CUBOS_CORE_MINIAUDIO
    return std::make_shared<MiniaudioContext>();
#else
    CUBOS_ERROR("Cannot create audio context: cubos-core was compiled without audio support");
    return nullptr;
#endif
}
