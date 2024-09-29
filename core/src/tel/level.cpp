#include <mutex>

#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/tel/level.hpp>

using cubos::core::reflection::EnumTrait;
using cubos::core::tel::Level;

CUBOS_REFLECT_EXTERNAL_IMPL(Level)
{
    return Type::create("cubos::core::tel::Level")
        .with(EnumTrait{}
                  .withVariant<Level::Trace>("Trace")
                  .withVariant<Level::Debug>("Debug")
                  .withVariant<Level::Info>("Info")
                  .withVariant<Level::Warn>("Warn")
                  .withVariant<Level::Error>("Error")
                  .withVariant<Level::Critical>("Critical"));
}

namespace
{
    /// @brief Private type which stores the state of the span tracer.
    struct State
    {
        std::mutex mutex;
        Level level{Level::Debug};
    };
} // namespace

/// @brief Telemetry level state singleton. Guarantees it is initialized exactly once, when needed.
/// @return Level state.
static State& state()
{
    static State state{};
    return state;
}

void cubos::core::tel::level(Level level)
{
    std::lock_guard<std::mutex> guard{state().mutex};
    state().level = level;
}

auto cubos::core::tel::level() -> Level
{
    std::lock_guard<std::mutex> guard{state().mutex};
    return state().level;
}