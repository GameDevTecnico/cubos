#include "penetration_constraint/plugin.hpp"

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>

#include "../fixed_substep/plugin.hpp"
#include "integration/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::physicsPrepareSolveTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsIntegrateVelocityTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSolveConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSolveUserConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSolveContactTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsIntegratePositionTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSolveRelaxConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSolveRelaxUserConstraintTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsSolveRelaxContactTag);
CUBOS_DEFINE_TAG(cubos::engine::physicsFinalizePositionTag);

using namespace cubos::engine;

void cubos::engine::solverPlugin(Cubos& cubos)
{
    cubos.depends(physicsPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(collisionsPlugin);

    // doesn't need to be after collisions but for now it should stay in case we do velocity based prediction for
    // collisions in the future
    cubos.tag(physicsPrepareSolveTag).after(collisionsTag).tagged(fixedStepTag);

    cubos.tag(physicsIntegrateVelocityTag)
        .after(physicsPrepareSolveTag)
        .after(physicsApplyForcesTag)
        .tagged(fixedSubstepTag);

    // Solve velocities using position bias
    // Not entirely sure we need this full separation yet but kept it to not forget this is an issue that needs
    // further research, for now only one is used either way
    cubos.tag(physicsSolveConstraintTag).after(physicsIntegrateVelocityTag).tagged(fixedSubstepTag);
    cubos.tag(physicsSolveUserConstraintTag).after(physicsSolveConstraintTag).tagged(fixedSubstepTag);
    cubos.tag(physicsSolveContactTag).after(physicsSolveUserConstraintTag).tagged(fixedSubstepTag);

    cubos.tag(physicsIntegratePositionTag).after(physicsSolveContactTag).tagged(fixedSubstepTag);

    // Relax biased velocities and impulses.
    // Same comment as above
    cubos.tag(physicsSolveRelaxConstraintTag).after(physicsIntegratePositionTag).tagged(fixedSubstepTag);
    cubos.tag(physicsSolveRelaxUserConstraintTag).after(physicsSolveRelaxConstraintTag).tagged(fixedSubstepTag);
    cubos.tag(physicsSolveRelaxContactTag).after(physicsSolveRelaxUserConstraintTag).tagged(fixedSubstepTag);

    cubos.tag(physicsFinalizePositionTag).after(physicsSolveRelaxContactTag).tagged(fixedStepTag);

    cubos.plugin(integrationPlugin);
    cubos.plugin(penetrationConstraintPlugin);
}
