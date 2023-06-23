#include <cubos/core/data/ser/serializer.hpp>
#include <cubos/core/reflection/primitive.hpp>

using cubos::core::data::Serializer;

void Serializer::write(const reflection::Type& type, const void* data)
{
    if (auto hook = mHooks.find(&type); hook != mHooks.end())
    {
        hook->second(data);
    }
    else
    {
        // Visit the type to call the appropriate write* function.
        auto prevInstance = mInstance;
        mInstance = data;
        type.accept(*this);
        mInstance = prevInstance;
    }
}

void Serializer::hook(const reflection::Type& type, Hook hook)
{
    mHooks[&type] = hook;
}

void Serializer::visit(const reflection::PrimitiveType& type)
{
    CUBOS_FAIL("Missing serialization hook for primitive type '{}'", type.name());
}

void Serializer::visit(const reflection::ObjectType& type)
{
    this->writeObject(type, mInstance);
}

void Serializer::visit(const reflection::VariantType& type)
{
    this->writeVariant(type, mInstance);
}

void Serializer::visit(const reflection::ArrayType& type)
{
    this->writeArray(type, mInstance);
}

void Serializer::visit(const reflection::DictionaryType& type)
{
    this->writeDictionary(type, mInstance);
}
