#include "plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

void cubos::engine::interpolationPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.component<Interpolated>();

    cubos.observer("set initial interpolation values")
        .onAdd<Interpolated>()
        .call([](Query<const Position&, const Rotation&, const Scale&, Interpolated&> query) {
            for (auto [position, rotation, scale, interpolated] : query)
            {
                interpolated.previousPosition = position.vec;
                interpolated.nextPosition = position.vec;
                interpolated.previousRotation = rotation.quat;
                interpolated.nextRotation = rotation.quat;
                interpolated.previousScale = scale.factor;
                interpolated.nextScale = scale.factor;
            }
        });

    cubos.system("do interpolation on Interpolated entities")
        .before(fixedStepTag)
        .call([](const FixedAccumulatedTime& acc, const DeltaTime& dt, const FixedDeltaTime& fdt,
                 Query<Position&, Rotation&, Scale&, const Interpolated&> query) {
            float alpha = (acc.value + dt.value()) / fdt.value;

            for (auto [position, rotation, scale, interpolated] : query)
            {
                position.vec = glm::mix(interpolated.previousPosition, interpolated.nextPosition, alpha);
                rotation.quat = glm::slerp(interpolated.previousRotation, interpolated.nextRotation, alpha);
                scale.factor = glm::mix(interpolated.previousScale, interpolated.nextScale, alpha);
            }
        });

    cubos.system("update Interpolated components")
        .tagged(fixedStepTag)
        .after(physicsFinalizePositionTag)
        .call([](Query<Interpolated&, const Position&, const Rotation&, const Scale&> query) {
            for (auto [interpolation, position, rotation, scale] : query)
            {
                interpolation.previousPosition = interpolation.nextPosition;
                interpolation.previousRotation = interpolation.nextRotation;
                interpolation.previousScale = interpolation.nextScale;
                interpolation.nextPosition = position.vec;
                interpolation.nextRotation = rotation.quat;
                interpolation.nextScale = scale.factor;
            }
        });
}