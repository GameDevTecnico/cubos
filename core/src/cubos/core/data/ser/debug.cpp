#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/array.hpp>
#include <cubos/core/reflection/dictionary.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/object.hpp>
#include <cubos/core/reflection/primitive.hpp>
#include <cubos/core/reflection/variant.hpp>

using cubos::core::data::DebugSerializer;

#define AUTO_HOOK(T, fmt)                                                                                              \
    this->hook<T>([this](const void* data) {                                                                           \
        typeName(reflection::reflect<T>(), false);                                                                     \
        mStream.printf((fmt), *static_cast<const T*>(data));                                                           \
    })

DebugSerializer::DebugSerializer(memory::Stream& stream, bool pretty, bool typeNames)
    : mStream(stream)
    , mPretty(pretty)
    , mTypeNames(typeNames)
    , mIndent(0)
{
    AUTO_HOOK(bool, "{}");
    AUTO_HOOK(char, "'{}'");
    AUTO_HOOK(int8_t, "{}");
    AUTO_HOOK(int16_t, "{}");
    AUTO_HOOK(int32_t, "{}");
    AUTO_HOOK(int64_t, "{}");
    AUTO_HOOK(uint8_t, "{}");
    AUTO_HOOK(uint16_t, "{}");
    AUTO_HOOK(uint32_t, "{}");
    AUTO_HOOK(uint64_t, "{}");
    AUTO_HOOK(float, "{}");
    AUTO_HOOK(double, "{}");
    AUTO_HOOK(std::string, "\"{}\"");
}

void DebugSerializer::writeObject(const reflection::ObjectType& type, const void* data)
{
    this->typeName(type, false);
    mStream.put('(');

    for (std::size_t i = 0; i < type.fields().size(); ++i)
    {
        auto& field = *type.fields()[i];

        mIndent += 1;
        this->separate();
        mStream.printf("{}: ", field.name());
        this->write(field.type(), field.get(data));
        mIndent -= 1;

        if (i != type.fields().size() - 1)
        {
            mStream.put(',');
        }
        else
        {
            this->separate();
        }
    }

    mStream.put(')');
}

void DebugSerializer::writeVariant(const reflection::VariantType& type, const void* data)
{
    this->typeName(type, false);
    auto& variant = type.variant(data);
    if (!mTypeNames)
    {
        // Only print the type name if we're not printing type names for all values.
        mStream.printf("{} ", variant.type().name());
    }
    this->write(variant.type(), variant.get(data));
}

void DebugSerializer::writeArray(const reflection::ArrayType& type, const void* data)
{
    this->typeName(type, type.length(data) > 0);
    mStream.put('[');

    for (std::size_t i = 0, end = type.length(data); i < end; ++i)
    {
        mIndent += 1;
        this->separate();
        this->write(type.elementType(), type.element(data, i));
        mIndent -= 1;

        if (i != end - 1)
        {
            mStream.put(',');
        }
        else
        {
            separate();
        }
    }

    mStream.put(']');
}

void DebugSerializer::writeDictionary(const reflection::DictionaryType& type, const void* data)
{
    typeName(type, type.length(data) > 0);
    mStream.put('{');

    auto iterator = type.iterator(data);
    while (iterator.valid())
    {
        mIndent += 1;
        separate();
        this->write(type.keyType(), iterator.key(data));
        mStream.print(": ");
        this->write(type.valueType(), iterator.value(data));
        iterator.increment(data);
        mIndent -= 1;

        if (iterator.valid())
        {
            mStream.put(',');
        }
        else
        {
            separate();
        }
    }

    mStream.put('}');
}

void DebugSerializer::separate()
{
    if (mPretty)
    {
        mStream.put('\n');
        for (int i = 0; i < mIndent * 2; ++i)
        {
            mStream.put(' ');
        }
    }
    else
    {
        mStream.put(' ');
    }
}

void DebugSerializer::typeName(const reflection::Type& type, bool shortened)
{
    if (mTypeNames)
    {
        mStream.printf("{} ", shortened ? type.shortName() : type.name());
    }
}
