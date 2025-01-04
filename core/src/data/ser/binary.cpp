#include <cubos/core/data/ser/binary.hpp>
#include <cubos/core/memory/endianness.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/traits/wrapper.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::data::BinarySerializer;
using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::MaskTrait;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;
using cubos::core::reflection::WrapperTrait;

#define AUTO_HOOK(casted, type)                                                                                        \
    this->hook<type>([this](const type& value) {                                                                       \
        auto castedValue = memory::toLittleEndian(static_cast<casted>(value));                                         \
        return mStream.writeExact(&castedValue, sizeof(castedValue));                                                  \
    })

BinarySerializer::BinarySerializer(memory::Stream& stream)
    : mStream(stream)
{
    this->hook<bool>([this](const bool& value) { return mStream.writeExact(value ? "\1" : "\0", 1); });
    this->hook<char>([this](const char& value) { return mStream.writeExact(&value, 1); });

    AUTO_HOOK(std::int8_t, signed char);
    AUTO_HOOK(std::int16_t, short);
    AUTO_HOOK(std::int32_t, int);
    AUTO_HOOK(std::int64_t, long);
    AUTO_HOOK(std::int64_t, long long);

    AUTO_HOOK(std::uint8_t, unsigned char);
    AUTO_HOOK(std::uint16_t, unsigned short);
    AUTO_HOOK(std::uint32_t, unsigned int);
    AUTO_HOOK(std::uint64_t, unsigned long);
    AUTO_HOOK(std::uint64_t, unsigned long long);

    AUTO_HOOK(float, float);
    AUTO_HOOK(double, double);

    this->hook<std::string>([this](const std::string& value) {
        if (!this->write(value.length()))
        {
            return false;
        }

        return mStream.writeExact(value.data(), value.length());
    });
}

bool BinarySerializer::decompose(const Type& type, const void* value)
{
    if (type.has<DictionaryTrait>())
    {
        const auto& trait = type.get<DictionaryTrait>();
        auto view = trait.view(value);

        if (!this->write(view.length()))
        {
            CUBOS_WARN("Could not serialize dictionary length");
            return false;
        }

        for (const auto& [entryKey, entryValue] : view)
        {
            if (!this->write(trait.keyType(), entryKey))
            {
                CUBOS_WARN("Could not serialize dictionary key");
                return false;
            }

            if (!this->write(trait.valueType(), entryValue))
            {
                CUBOS_WARN("Could not serialize dictionary value");
                return false;
            }
        }
    }
    else if (type.has<ArrayTrait>())
    {
        const auto& trait = type.get<ArrayTrait>();
        auto view = trait.view(value);

        if (!this->write(view.length()))
        {
            CUBOS_WARN("Could not serialize array length");
            return false;
        }

        for (const auto* element : view)
        {
            if (!this->write(trait.elementType(), element))
            {
                CUBOS_WARN("Could not serialize array element");
                return false;
            }
        }
    }
    else if (type.has<EnumTrait>())
    {
        const auto& trait = type.get<EnumTrait>();
        if (!this->write(trait.variant(value).name()))
        {
            CUBOS_WARN("Could not serialize enum variant");
            return false;
        }
    }
    else if (type.has<MaskTrait>())
    {
        const auto& trait = type.get<MaskTrait>();
        std::size_t i = 0;
        for (const auto& bit : trait.view(value))
        {
            (void)bit;
            ++i;
        }

        if (!this->write(i))
        {
            CUBOS_WARN("Could not serialize mask bit count");
            return false;
        }

        for (const auto& bit : trait.view(value))
        {
            if (!this->write(bit.name()))
            {
                CUBOS_WARN("Could not serialize mask bit");
                return false;
            }
        }
    }
    else if (type.has<FieldsTrait>())
    {
        const auto& trait = type.get<FieldsTrait>();
        for (const auto& [field, fieldValue] : trait.view(value))
        {
            if (!this->write(field->type(), fieldValue))
            {
                CUBOS_WARN("Could not serialize field {}", field->name());
                return false;
            }
        }
    }
    else if (type.has<WrapperTrait>())
    {
        const auto& trait = type.get<WrapperTrait>();
        if (!this->write(trait.type(), trait.value(value)))
        {
            CUBOS_ERROR("Could not serialize wrapper of type {}", trait.type().name());
            return false;
        }
    }
    else if (type.has<StringConversionTrait>())
    {
        const auto& trait = type.get<StringConversionTrait>();
        if (!this->write(trait.into(value)))
        {
            CUBOS_WARN("Could not serialize string conversion");
            return false;
        }
    }
    else
    {
        CUBOS_WARN("Type {} doesn't have any of the supported traits", type.name());
        return false;
    }

    return true;
}
