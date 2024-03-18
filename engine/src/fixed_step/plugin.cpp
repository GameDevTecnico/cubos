#include <cubos/engine/fixed_step/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::fixedStepTag);

namespace
{
    struct AccumulatedTime
    {
        float value = 0.0F;
    };
} // namespace

void cubos::engine::fixedStepPlugin(Cubos& cubos)
{
    cubos.resource<AccumulatedTime>();
    cubos.resource<FixedDeltaTime>();

    cubos.tag(fixedStepTag).repeatWhile([](AccumulatedTime& timer, const FixedDeltaTime& step) {
        if (timer.value >= step.value)
        {
            timer.value -= step.value;
            return true;
        }
        return false;
    });

    cubos.system("accumulate time resource").before(fixedStepTag).call([](AccumulatedTime& timer, const DeltaTime& dt) {
        timer.value += dt.value;
    });
}
