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
    if (layerNumber > 31)
    {
        return;
    }

    value = (value & ~(1 << layerNumber)) | (static_cast<uint32_t>(newValue) << layerNumber);
}

bool cubos::engine::CollisionLayers::getLayerValue(unsigned int layerNumber) const
{
    if (layerNumber > 31)
    {
        return false;
    }

    return static_cast<bool>((value >> layerNumber) & 1);
}
