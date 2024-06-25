#include <stack>

#include <cubos/core/tel/tracing.hpp>

using cubos::core::tel::SpanManager;

namespace
{
    /// @brief Private type which stores the state of the span tracer.
    struct State
    {
        std::stack<SpanManager::Span*> spans;
    };
} // namespace

/// @brief Span tracer state singleton. Guarantees it is initialized exactly once, per thread, when needed.
/// @return Tracer state.
static State& state()
{
    static thread_local State state{};
    return state;
}

SpanManager::Span::Span(const std::string& name, std::size_t level, const std::string& file, std::size_t line)
    : id(std::this_thread::get_id())
    , mLevel(level)
    , mName(SpanManager::nextName(name))
    , mFile(file)
    , mLine(line)
    , mStart(std::chrono::time_point<std::chrono::high_resolution_clock>::clock::now())
{
    SpanManager::enter(this);
    printf("START %s \n", mName.c_str());
}

SpanManager::Span::~Span()
{
    auto now = std::chrono::time_point<std::chrono::high_resolution_clock>::clock::now();
    std::chrono::duration<double, std::milli> elapsed = now - mStart;
    // TODO store metric
    printf("END %s \n", mName.c_str());
    SpanManager::exit();
}

std::string SpanManager::Span::name()
{
    return mName;
}

SpanManager::Span* SpanManager::current()
{
    return state().spans.empty() ? nullptr : state().spans.top();
}

std::string SpanManager::nextName(std::string name)
{
    if (current() != nullptr)
    {
        name = current()->name() + ":" + name;
    }

    return name;
}

std::size_t SpanManager::count()
{
    return state().spans.size();
}

void SpanManager::enter(Span* s)
{
    state().spans.push(s);
}

void SpanManager::exit()
{
    if (!state().spans.empty())
    {
        state().spans.pop();
    }
}