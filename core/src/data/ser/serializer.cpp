#include <cubos/core/data/ser/serializer.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::data::Serializer;

bool Serializer::write(const reflection::Type& type, const void* value)
{
    if (auto it = mHooks.find(&type); it != mHooks.end())
    {
        if (!it->second(value))
        {
            CUBOS_WARN("Serialization hook for type {} failed", type.name());
            return false;
        }
    }
    else if (!this->decompose(type, value))
    {

        CUBOS_WARN("Serialization decomposition for type {} failed", type.name());
        return false;
    }

    return true;
}

void Serializer::hook(const reflection::Type& type, Hook hook)
{
    if (auto it = mHooks.find(&type); it != mHooks.end())
    {
        CUBOS_WARN("Hook for type {} already exists, overwriting", type.name());
        mHooks.erase(it);
    }

    mHooks.emplace(&type, memory::move(hook));
}
