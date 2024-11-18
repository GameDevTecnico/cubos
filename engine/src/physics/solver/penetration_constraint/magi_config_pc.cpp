#include "magi_config_pc.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::MagiConfigPC)
{
    return core::ecs::TypeBuilder<MagiConfigPC>("cubos::engine::MagiConfigPC")
        .withField("biasMax", &MagiConfigPC::biasMax)
        .withField("contactHertzMin", &MagiConfigPC::contactHertzMin)
        .withField("kNormal", &MagiConfigPC::kNormal)
        .withField("kFriction", &MagiConfigPC::kFriction)
        .build();
}