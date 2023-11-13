#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::data::DebugSerializer;
using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;

#define AUTO_HOOK(type, ...)                                                                                           \
    this->hook<type>([this](const type& value) {                                                                       \
        __VA_ARGS__;                                                                                                   \
        return true;                                                                                                   \
    })

DebugSerializer::DebugSerializer(memory::Stream& stream)
    : mStream(stream)
{
    AUTO_HOOK(bool, mStream.print(value ? "true" : "false"));
    AUTO_HOOK(char, {
        mStream.put('\'');
        mStream.put(value);
        mStream.put('\'');
    });
    AUTO_HOOK(int8_t, mStream.print(value));
    AUTO_HOOK(int16_t, mStream.print(value));
    AUTO_HOOK(int32_t, mStream.print(value));
    AUTO_HOOK(int64_t, mStream.print(value));
    AUTO_HOOK(uint8_t, mStream.print(value));
    AUTO_HOOK(uint16_t, mStream.print(value));
    AUTO_HOOK(uint32_t, mStream.print(value));
    AUTO_HOOK(uint64_t, mStream.print(value));
    AUTO_HOOK(float, mStream.print(value));
    AUTO_HOOK(double, mStream.print(value));
    AUTO_HOOK(std::string, mStream.printf("\"{}\"", value));
}

void DebugSerializer::pretty(bool pretty)
{
    mPretty = pretty;
}

void DebugSerializer::separate(bool first)
{
    if (mPretty)
    {
        if (!first)
        {
            mStream.put(',');
        }

        mStream.put('\n');
        for (int i = 0; i < mLevel * 2; ++i)
        {
            mStream.put(' ');
        }
    }
    else if (!first)
    {
        mStream.print(", ");
    }
}

bool DebugSerializer::decompose(const Type& type, const void* value)
{
    bool success = true;
    bool first = true;

    if (type.has<DictionaryTrait>())
    {
        const auto& trait = type.get<DictionaryTrait>();

        mStream.put('{');
        mLevel += 1;

        for (auto [entryKey, entryValue] : trait.view(value))
        {
            this->separate(first);
            first = false;

            if (!this->write(trait.keyType(), entryKey))
            {
                CUBOS_WARN("Could not write dictionary key");
                mStream.put('?');
                success = false;
            }

            mStream.print(": ");

            if (!this->write(trait.valueType(), entryValue))
            {
                CUBOS_WARN("Could not write dictionary value");
                mStream.put('?');
                success = false;
            }
        }

        mLevel -= 1;
        this->separate(true);
        mStream.put('}');
    }
    else if (type.has<ArrayTrait>())
    {
        const auto& trait = type.get<ArrayTrait>();

        mStream.put('[');
        mLevel += 1;

        for (const auto* element : trait.view(value))
        {
            this->separate(first);
            first = false;

            if (!this->write(trait.elementType(), element))
            {
                CUBOS_WARN("Could not write array element");
                mStream.put('?');
                success = false;
            }
        }

        mLevel -= 1;
        this->separate(true);
        mStream.put(']');
    }
    else if (type.has<FieldsTrait>())
    {
        const auto& trait = type.get<FieldsTrait>();

        mStream.put('(');
        mLevel += 1;

        for (const auto& [field, fieldValue] : trait.view(value))
        {
            this->separate(first);
            first = false;

            mStream.printf("{}: ", field->name());

            if (!this->write(field->type(), fieldValue))
            {
                CUBOS_WARN("Could not write field");
                mStream.put('?');
                success = false;
            }
        }

        mLevel -= 1;
        this->separate(true);
        mStream.put(')');
    }
    else if (type.has<StringConversionTrait>())
    {
        const auto& trait = type.get<StringConversionTrait>();
        mStream.print(trait.into(value));
    }
    else
    {
        CUBOS_WARN("Type {} doesn't have any of the supported traits", type.name());
        mStream.put('?');
        success = false;
    }

    return success;
}
