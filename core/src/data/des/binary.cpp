#include <cubos/core/data/des/binary.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/memory/endianness.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::data::BinaryDeserializer;
using cubos::core::memory::AnyValue;
using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::MaskTrait;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;

// NOLINTBEGIN(bugprone-macro-parentheses)
#define AUTO_HOOK(casted, type)                                                                                        \
    this->hook<type>([this](type& value) {                                                                             \
        casted castedValue;                                                                                            \
        if (!mStream.readExact(&castedValue, sizeof(castedValue)))                                                     \
        {                                                                                                              \
            return false;                                                                                              \
        }                                                                                                              \
        value = static_cast<type>(memory::fromLittleEndian(castedValue));                                              \
        return true;                                                                                                   \
    })
// NOLINTEND(bugprone-macro-parentheses)

BinaryDeserializer::BinaryDeserializer(memory::Stream& stream)
    : mStream(stream)
{
    this->hook<bool>([this](bool& value) {
        value = mStream.get() != '\0';
        return !mStream.eof();
    });
    this->hook<char>([this](char& value) { return mStream.readExact(&value, 1); });

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

    this->hook<std::string>([this](std::string& value) {
        std::size_t length;
        if (!this->read(length))
        {
            return false;
        }

        value.resize(length);
        return mStream.readExact(value.data(), length);
    });
}

bool BinaryDeserializer::decompose(const Type& type, void* value)
{
    if (type.has<DictionaryTrait>())
    {
        const auto& trait = type.get<DictionaryTrait>();
        auto view = trait.view(value);
        view.clear();

        std::size_t length;
        if (!this->read(length))
        {
            CUBOS_ERROR("Could not deserialize dictionary length");
            return false;
        }

        auto key = AnyValue::defaultConstruct(trait.keyType());
        for (std::size_t i = 0; i < length; ++i)
        {
            if (!this->read(trait.keyType(), key.get()))
            {
                CUBOS_ERROR("Could not deserialize dictionary key");
                return false;
            }

            view.insertDefault(key.get());
            auto entry = view.find(key.get());
            CUBOS_ASSERT(entry != view.end(), "We just inserted the entry, where did it go?");

            if (!this->read(trait.valueType(), entry->value))
            {
                CUBOS_ERROR("Could not deserialize dictionary value");
                return false;
            }
        }
    }
    else if (type.has<ArrayTrait>())
    {
        const auto& trait = type.get<ArrayTrait>();
        auto view = trait.view(value);

        std::size_t length;
        if (!this->read(length))
        {
            CUBOS_ERROR("Could not deserialize array length");
            return false;
        }
        view.resize(length);

        for (auto* element : view)
        {
            if (!this->read(trait.elementType(), element))
            {
                CUBOS_ERROR("Could not deserialize array element");
                return false;
            }
        }
    }
    else if (type.has<EnumTrait>())
    {
        const auto& trait = type.get<EnumTrait>();

        std::string name;
        if (!this->read(name))
        {
            CUBOS_ERROR("Could not deserialize enum");
            return false;
        }

        if (!trait.contains(name))
        {
            CUBOS_ERROR("Could not deserialize enum, no such variant '{}'", name);
            return false;
        }

        trait.at(name).set(value);
    }
    else if (type.has<MaskTrait>())
    {
        const auto& trait = type.get<MaskTrait>();

        std::size_t bitCount = 0;
        if (!this->read(bitCount))
        {
            CUBOS_ERROR("Could not deserialize mask bit count");
            return false;
        }

        for (auto& bit : trait)
        {
            bit.clear(value);
        }

        for (std::size_t i = 0; i < bitCount; ++i)
        {
            std::string bit;
            if (!this->read(bit))
            {
                CUBOS_ERROR("Could not deserialize mask bit name");
                return false;
            }

            if (!trait.contains(bit))
            {
                CUBOS_ERROR("Could not deserialize mask, no such bit '{}'", bit);
                return false;
            }

            trait.at(bit).set(value);
        }
    }
    else if (type.has<FieldsTrait>())
    {
        const auto& trait = type.get<FieldsTrait>();
        for (const auto& [field, fieldValue] : trait.view(value))
        {
            if (!this->read(field->type(), fieldValue))
            {
                CUBOS_ERROR("Could not deserialize field '{}'", field->name());
                return false;
            }
        }
    }
    else if (type.has<StringConversionTrait>())
    {
        const auto& trait = type.get<StringConversionTrait>();

        std::string string;
        if (!this->read(string))
        {
            CUBOS_ERROR("Could not deserialize string conversion");
            return false;
        }

        if (!trait.from(value, string))
        {
            CUBOS_ERROR("Could not deserialize string conversion, string '{}' is not a valid conversion", string);
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
