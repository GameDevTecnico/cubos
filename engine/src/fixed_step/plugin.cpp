#include <cubos/engine/fixed_step/plugin.hpp>

CUBOS_DEFINE_TAG(cubos::engine::fixedStepTag);

namespace
{
    struct AccumulatedTime
    {
        CUBOS_ANONYMOUS_REFLECT(AccumulatedTime);

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

    cubos.system("accumulate time resource")
        .before(fixedStepTag)
        .call([](AccumulatedTime& timer, const DeltaTime& dt, const FixedDeltaTime& step) {
            timer.value += std::min(dt.value(), 2 * step.value);
        });
}
