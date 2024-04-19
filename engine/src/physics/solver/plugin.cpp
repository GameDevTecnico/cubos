#include "../../collisions/narrow_phase/plugin.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>

#include "penetration_constraint/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::physicsSolveTag);

using namespace cubos::engine;

void cubos::engine::solverPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(physicsPlugin);

    cubos.tag(physicsSolveTag)
        .after(physicsSimulationSubstepsIntegrateTag)
        .before(physicsSimulationSubstepsCorrectPositionTag)
        .tagged(fixedStepTag);

    cubos.plugin(penetrationConstraintPlugin);
}
