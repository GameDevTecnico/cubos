#include <cubos/core/data/des/deserializer.hpp>
#include <cubos/core/reflection/primitive.hpp>

using cubos::core::data::Deserializer;

bool Deserializer::read(const reflection::Type& type, void* data)
{
    if (auto hook = mHooks.find(&type); hook != mHooks.end())
    {
        return hook->second(data);
    }

    // Visit the type to call the appropriate read* function.
    auto prevInstance = mInstance;
    mInstance = data;
    type.accept(*this);
    if (mInstance == nullptr)
    {
        CUBOS_ERROR("Failed to deserialize data of type '{}'", type.name());
        return false;
    }
    mInstance = prevInstance;

    return true;
}

void Deserializer::visit(const reflection::PrimitiveType& type)
{
    CUBOS_FAIL("Missing deserialization hook for primitive type '{}'", type.name());
}

void Deserializer::visit(const reflection::ObjectType& type)
{
    if (!this->readObject(type, mInstance))
    {
        mInstance = nullptr;
    }
}

void Deserializer::visit(const reflection::VariantType& type)
{
    if (!this->readVariant(type, mInstance))
    {
        mInstance = nullptr;
    }
}

void Deserializer::visit(const reflection::ArrayType& type)
{
    if (!this->readArray(type, mInstance))
    {
        mInstance = nullptr;
    }
}

void Deserializer::visit(const reflection::DictionaryType& type)
{
    if (!this->readDictionary(type, mInstance))
    {
        mInstance = nullptr;
    }
}
