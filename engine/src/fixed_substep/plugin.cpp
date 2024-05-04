#include "../collisions/narrow_phase/plugin.hpp"

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/fixed_substep/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::fixedSubstepTag);

namespace
{
    struct SubstepsCount
    {
        int value = 0;
    };
} // namespace

void cubos::engine::fixedSubstepPlugin(Cubos& cubos)
{
    cubos.depends(fixedStepPlugin);
    cubos.depends(collisionsPlugin);
    cubos.resource<SubstepsCount>();
    cubos.resource<Substeps>();

    cubos.tag(fixedSubstepTag)
        .after(collisionsNarrowTag)
        .repeatWhile([](SubstepsCount& count, const Substeps& substeps) {
            count.value += 1;
            return count.value <= substeps.value;
        })
        .tagged(fixedStepTag);

    cubos.system("reset substeps tag").before(fixedSubstepTag).tagged(fixedStepTag).call([](SubstepsCount& count) {
        count.value = 0;
    });
}
