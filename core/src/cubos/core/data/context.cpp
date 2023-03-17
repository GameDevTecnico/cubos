#include <cubos/core/data/context.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core::data;

Context::~Context()
{
    for (auto& entry : entries)
    {
        if (entry.subContext == nullptr)
        {
            entry.destructor(entry.data);
        }
    }
}

void Context::pushSubContext(Context& context)
{
    this->entries.push_back({&context, typeid(Context), nullptr, nullptr});
}

void Context::pushAny(std::type_index type, void* data, void (*destructor)(void*))
{
    entries.push_back({nullptr, type, data, destructor});
}

void Context::pop()
{
    entries.back().destructor(entries.back().data);
    entries.pop_back();
}

void* Context::getAny(std::type_index type) const
{
    auto res = this->tryGetAny(type);
    if (res == nullptr)
    {
        CUBOS_CRITICAL("Type '{}' requested in serialization context, but not present", type.name());
        abort();
    }

    return res;
}

void* Context::tryGetAny(std::type_index type) const
{
    for (auto it = entries.rbegin(); it != entries.rend(); ++it)
    {
        if (it->subContext != nullptr)
        {
            auto res = it->subContext->tryGetAny(type);
            if (res != nullptr)
            {
                return res;
            }
        }
        else if (it->type == type)
        {
            return it->data;
        }
    }

    return nullptr;
}
