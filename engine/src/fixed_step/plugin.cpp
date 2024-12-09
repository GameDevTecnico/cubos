#include <cubos/engine/fixed_step/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::fixedStepTag);

void cubos::engine::fixedStepPlugin(Cubos& cubos)
{
    cubos.resource<FixedAccumulatedTime>();
    cubos.resource<FixedDeltaTime>();

    cubos.tag(fixedStepTag).repeatWhile([](FixedAccumulatedTime& timer, const FixedDeltaTime& step) {
        if (timer.value >= step.value)
        {
            timer.value -= step.value;
            return true;
        }
        return false;
    });

    cubos.system("accumulate time resource")
        .before(fixedStepTag)
        .call([](FixedAccumulatedTime& timer, const DeltaTime& dt, const FixedDeltaTime& step) {
            timer.value += std::min(dt.value(), 2 * step.value);
        });
}
