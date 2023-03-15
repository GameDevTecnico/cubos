#include <cubos/core/data/context.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core::data;

Context::~Context()
{
    for (auto& entry : entries)
    {
        entry.destructor(entry.data);
    }
}

void Context::pushAny(std::type_index type, void* data, void (*destructor)(void*))
{
    entries.push_back({type, data, destructor});
}

void Context::pop()
{
    entries.back().destructor(entries.back().data);
    entries.pop_back();
}

void* Context::getAny(std::type_index type)
{
    for (auto it = entries.rbegin(); it != entries.rend(); ++it)
    {
        if (it->type == type)
        {
            return it->data;
        }
    }

    CUBOS_CRITICAL("Type '{}' requested in serialization context, but not present", type.name());
    abort();
}
