#include <cubos/core/data/old/context.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>

using namespace cubos::core::data::old;

Context::~Context()
{
    for (auto& entry : mEntries)
    {
        if (entry.subContext == nullptr)
        {
            entry.destructor(entry.data);
        }
    }
}

void Context::pushSubContext(Context& context)
{
    mEntries.push_back({&context, typeid(Context), nullptr, nullptr});
}

void Context::pushAny(std::type_index type, void* data, void (*destructor)(void*))
{
    mEntries.push_back({nullptr, type, data, destructor});
}

void Context::pop()
{
    if (mEntries.back().subContext == nullptr)
    {
        mEntries.back().destructor(mEntries.back().data);
    }

    mEntries.pop_back();
}

void* Context::getAny(std::type_index type) const
{
    auto* res = this->tryGetAny(type);
    if (res == nullptr)
    {
        CUBOS_CRITICAL("Type '{}' requested in serialization context, but not present", type.name());
        abort();
    }

    return res;
}

void* Context::tryGetAny(std::type_index type) const
{
    for (auto it = mEntries.rbegin(); it != mEntries.rend(); ++it)
    {
        if (it->subContext != nullptr)
        {
            auto* res = it->subContext->tryGetAny(type);
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
