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

CUBOS_DEFINE_TAG(cubos::engine::interpolationResetTag);

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

    cubos.tag(interpolationResetTag).runIf([](FixedAccumulatedTime& timer, const FixedDeltaTime& step) {
        if (timer.value >= step.value)
        {
            return true;
        }
        return false;
    });

    cubos.observer("set up Interpolated")
        .onAdd<Interpolated>()
        .call([](Query<Entity, Interpolated&, const Position&, const Rotation&, const Scale&> query) {
            for (auto [ent, interp, position, rotation, scale] : query)
            {
                interp.previousPosition = position.vec;
                // interp.previousRotation = rotation.quat;
                interp.previousScale = scale.factor;
                interp.nextPosition = position.vec;
                // interp.nextRotation = rotation.quat;
                interp.nextScale = scale.factor;
            }
        });

    cubos.system("set position to real value before physics update")
        .tagged(interpolationResetTag)
        .before(transformUpdateTag)
        .call([](Query<Position&, const Rotation&, const Scale&, const Interpolated&> query) {
            for (auto [position, rotation, scale, interpolated] : query)
            {
                position.vec = interpolated.nextPosition;
            }
        });

    cubos.system("prepare before update")
        .tagged(fixedStepTag)
        .before(physicsFinalizePositionTag)
        .call([](Query<const Position&, const Rotation&, const Scale&, Interpolated&> query) {
            for (auto [position, rotation, scale, interpolated] : query)
            {
                interpolated.previousPosition = interpolated.nextPosition;
                // interpolated.previousRotation = interpolated.nextRotation;
                interpolated.previousScale = interpolated.nextScale;
            }
        });

    cubos.system("do interpolation on Interpolated entities")
        .after(fixedStepTag)
        .call([](const FixedAccumulatedTime& acc, const DeltaTime& dt, const FixedDeltaTime& fdt,
                 Query<Position&, Rotation&, Scale&, Interpolated&> query) {
            float alpha = (acc.value + dt.value()) / fdt.value;

            for (auto [position, rotation, scale, interpolated] : query)
            {

                if (position.vec == interpolated.currentPosition)
                {
                    position.vec = glm::mix(interpolated.previousPosition, interpolated.nextPosition, alpha);
                }
                else
                {
                    interpolated.previousPosition = position.vec;
                    interpolated.nextPosition = position.vec;
                }
                interpolated.currentPosition = position.vec;

                /*
                if (rotation.quat == interpolated.currentRotation)
                {
                    rotation.quat = glm::slerp(interpolated.previousRotation, interpolated.nextRotation, alpha);
                }
                else
                {
                    interpolated.previousRotation = rotation.quat;
                    interpolated.nextRotation = rotation.quat;
                }
                interpolated.currentRotation = rotation.quat;
                */

                if (scale.factor == interpolated.nextScale)
                {
                    scale.factor = glm::mix(interpolated.previousScale, interpolated.nextScale, alpha);
                }
                else
                {
                    interpolated.previousScale = scale.factor;
                    interpolated.nextScale = scale.factor;
                }
                interpolated.currentScale = scale.factor;
            }
        });
}