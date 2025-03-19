#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/collision_mask.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CollisionMask)
{
    return core::ecs::TypeBuilder<CollisionMask>("cubos::engine::CollisionMask")
        .withField("value", &CollisionMask::value)
        .build();
}

void cubos::engine::CollisionMask::setLayerValue(unsigned int layerNumber, bool newValue)
{
    CUBOS_ASSERT(layerNumber < 32, "Collision mask layer number must be between 0 and 31");
    value = (value & ~(1 << layerNumber)) | (static_cast<uint32_t>(newValue) << layerNumber);
}

bool cubos::engine::CollisionMask::getLayerValue(unsigned int layerNumber) const
{
    CUBOS_ASSERT(layerNumber < 32, "Collision mask layer number must be between 0 and 31");
    return static_cast<bool>((value >> layerNumber) & 1);
}
