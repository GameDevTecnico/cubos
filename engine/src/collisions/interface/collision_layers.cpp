#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/collision_layers.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::CollisionLayers)
{
    return core::ecs::TypeBuilder<CollisionLayers>("cubos::engine::CollisionLayers")
        .withField("value", &CollisionLayers::value)
        .build();
}

void cubos::engine::CollisionLayers::setLayerValue(unsigned int layerNumber, bool newValue)
{
    CUBOS_ASSERT(layerNumber < 32, "Collision layer number must be between 0 and 31");
    value = (value & ~(static_cast<uint32_t>(1) << static_cast<uint32_t>(layerNumber))) |
            (static_cast<uint32_t>(newValue) << layerNumber);
}

bool cubos::engine::CollisionLayers::getLayerValue(unsigned int layerNumber) const
{
    CUBOS_ASSERT(layerNumber < 32, "Collision layer number must be between 0 and 31");
    return static_cast<bool>((value >> layerNumber) & 1);
}
