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
    if (layerNumber > 31)
    {
        return;
    }

    value = (value & ~(1 << layerNumber)) | (static_cast<uint32_t>(newValue) << layerNumber);
}

bool cubos::engine::CollisionMask::getLayerValue(unsigned int layerNumber) const
{
    if (layerNumber > 31)
    {
        return false;
    }

    return static_cast<bool>((value >> layerNumber) & 1);
}
