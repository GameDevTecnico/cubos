#include <cubos/core/data/des/deserializer.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::data::Deserializer;

bool Deserializer::read(const reflection::Type& type, void* value)
{
    if (auto it = mHooks.find(&type); it != mHooks.end())
    {
        if (!it->second(value))
        {
            CUBOS_WARN("Deserialization hook for type '{}' failed", type.name());
            return false;
        }
    }
    else if (!this->decompose(type, value))
    {
        CUBOS_WARN("Deserialization decomposition for type '{}' failed", type.name());
        return false;
    }

    return true;
}

void Deserializer::hook(const reflection::Type& type, Hook hook)
{
    if (auto it = mHooks.find(&type); it != mHooks.end())
    {
        CUBOS_WARN("Hook for type '{}' already exists, overwriting", type.name());
        mHooks.erase(it);
    }

    mHooks.emplace(&type, memory::move(hook));
}
