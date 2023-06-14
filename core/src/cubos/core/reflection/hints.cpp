#include <cubos/core/reflection/hints.hpp>

using cubos::core::reflection::Hints;
using cubos::core::reflection::TypeMap;

void Hints::add(const Type& type, std::shared_ptr<void>&& data)
{
    if (!mHints.insert(type, std::move(data)))
    {
        CUBOS_WARN("Duplicate hint of type {}, ignoring", type.name());
    }
}

const void* Hints::get(const Type& type) const
{
    if (mHints.contains(type))
    {
        return mHints.at(type).get();
    }
    else
    {
        return nullptr;
    }
}
