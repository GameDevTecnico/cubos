#include <cubos/core/ecs/component/storage.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::Storage;
using cubos::core::reflection::Type;

Storage::Storage(const Type& type)
    : mData{type}
{
    mConstructibleTrait = &type.get<reflection::ConstructibleTrait>();
}

void Storage::insert(uint32_t index, void* value)
{
    while (mData.size() <= static_cast<std::size_t>(index))
    {
        mData.pushDefault();
    }

    mConstructibleTrait->destruct(mData.at(index));
    mConstructibleTrait->moveConstruct(mData.at(index), value);
}

void Storage::erase(uint32_t index)
{
    if (static_cast<std::size_t>(index) < mData.size())
    {
        void* value = mData.at(static_cast<std::size_t>(index));
        mConstructibleTrait->destruct(value);
        mConstructibleTrait->defaultConstruct(value);
    }
}

void* Storage::get(uint32_t index)
{
    return mData.at(static_cast<std::size_t>(index));
}

const void* Storage::get(uint32_t index) const
{
    return mData.at(static_cast<std::size_t>(index));
}

const Type& Storage::type() const
{
    return mData.elementType();
}
