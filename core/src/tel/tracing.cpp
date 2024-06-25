#include <chrono>
#include <sstream>
#include <stack>
#include <thread>
#include <utility>

#include <cubos/core/tel/logging.hpp>
#include <cubos/core/tel/metrics.hpp>
#include <cubos/core/tel/tracing.hpp>

using cubos::core::tel::Level;
using cubos::core::tel::SpanGuard;
using cubos::core::tel::SpanId;
using cubos::core::tel::SpanManager;

namespace
{
    /// @brief Private type which stores the state of the span tracer.
    struct State
    {
        std::stack<SpanId> spans;
        std::size_t spanInstanceId{0};

        State()
        {
            // create main thread span if not present
            if (this->spans.empty())
            {
                std::ostringstream oss;
                oss << std::this_thread::get_id();

                auto span = SpanId{.id = this->spanInstanceId, .path = "thread" + oss.str()};
                this->spans.push(span);
                this->spanInstanceId++;
            }
        }
    };
} // namespace

/// @brief Span tracer state singleton. Guarantees it is initialized exactly once, per thread, when needed.
/// @return Tracer state.
static State& state()
{
    static thread_local State state{};
    return state;
}

SpanGuard::~SpanGuard()
{
    SpanManager::end();
}

SpanId SpanManager::current()
{
    return state().spans.top();
}

void SpanManager::begin(const std::string& name, Level level)
{
    if (level >= cubos::core::tel::level())
    {
        auto span = SpanId{.id = state().spanInstanceId, .path = state().spans.top().path + ":" + name};
        state().spans.push(span);
        state().spanInstanceId++;

#ifdef CUBOS_PROFILING
        // Register 'begin' span time
        std::chrono::duration<double, std::milli> begin =
            std::chrono::time_point<std::chrono::high_resolution_clock>::clock::now().time_since_epoch();
        CUBOS_METRIC("begin", begin.count());
#endif
    }
    else
    {
        // push duplicate
        state().spans.push(SpanManager::current());
    }
}

void SpanManager::end()
{
    CUBOS_ASSERT(state().spans.size() > 1, "Can't exit root span! (did you forget to open a span?)");

    auto poppedSpan = state().spans.top();
    state().spans.pop();

    if (poppedSpan.id == SpanManager::current().id)
    {
        return;
    }

#ifdef CUBOS_PROFILING
    // Register 'end' span time
    std::chrono::duration<double, std::milli> end =
        std::chrono::time_point<std::chrono::high_resolution_clock>::clock::now().time_since_epoch();
    CUBOS_METRIC("end", end.count());
#endif
}