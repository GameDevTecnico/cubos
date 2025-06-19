#include <glm/glm.hpp>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/interpolation/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(cubos::engine::Interpolated)
{
    return cubos::core::ecs::TypeBuilder<Interpolated>("cubos::engine::Interpolated").build();
}

void cubos::engine::interpolationPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.component<Interpolated>();

    cubos.observer("add Interpolated when Velocity is added")
        .onAdd<Velocity>()
        .without<Interpolated>()
        .call([](Commands cmds, Query<Entity, const Velocity&, const Position&, const Rotation&, const Scale&> query) {
            for (auto [ent, _, position, rotation, scale] : query)
            {
                cmds.add(ent, Interpolated{.previousPosition = position.vec,
                                           .previousRotation = rotation.quat,
                                           .previousScale = scale.factor,
                                           .nextPosition = position.vec,
                                           .nextRotation = rotation.quat,
                                           .nextScale = scale.factor});
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