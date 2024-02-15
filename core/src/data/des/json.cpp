#include <cubos/core/data/des/json.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::data::JSONDeserializer;
using cubos::core::memory::AnyValue;
using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::StringConversionTrait;
using cubos::core::reflection::Type;

// NOLINTBEGIN(bugprone-macro-parentheses)
#define AUTO_HOOK(T, fromString)                                                                                       \
    this->hook<T>([this](T& value) {                                                                                   \
        if (mIsKey)                                                                                                    \
        {                                                                                                              \
            try                                                                                                        \
            {                                                                                                          \
                value = static_cast<T>(fromString(mIterator.key()));                                                   \
            }                                                                                                          \
            catch (std::invalid_argument&)                                                                             \
            {                                                                                                          \
                CUBOS_WARN("Cannot deserialize {} from JSON key {}: invalid string", #T, mIterator.key());             \
                return false;                                                                                          \
            }                                                                                                          \
            catch (std::out_of_range&)                                                                                 \
            {                                                                                                          \
                CUBOS_WARN("Cannot deserialize {} from JSON key {}: out of range", #T, mIterator.key());               \
                return false;                                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            try                                                                                                        \
            {                                                                                                          \
                value = mIterator.value();                                                                             \
            }                                                                                                          \
            catch (nlohmann::json::exception&)                                                                         \
            {                                                                                                          \
                CUBOS_WARN("Cannot deserialize {} from a JSON {}", #T, mIterator->type_name());                        \
                return false;                                                                                          \
            }                                                                                                          \
        }                                                                                                              \
        return true;                                                                                                   \
    })
// NOLINTEND(bugprone-macro-parentheses)

JSONDeserializer::JSONDeserializer()
{
    AUTO_HOOK(bool, [](const std::string& s) {
        if (s == "true")
        {
            return true;
        }

        if (s == "false")
        {
            return false;
        }

        throw std::invalid_argument{s};
    });

    AUTO_HOOK(signed char, std::stol);
    AUTO_HOOK(short, std::stol);
    AUTO_HOOK(int, std::stol);
    AUTO_HOOK(long, std::stol);
    AUTO_HOOK(long long, std::stoll);

    AUTO_HOOK(unsigned char, std::stoul);
    AUTO_HOOK(unsigned short, std::stoul);
    AUTO_HOOK(unsigned int, std::stoul);
    AUTO_HOOK(unsigned long, std::stoul);
    AUTO_HOOK(unsigned long long, std::stoull);

    AUTO_HOOK(float, std::stof);
    AUTO_HOOK(double, std::stod);
}

void JSONDeserializer::feed(nlohmann::json json)
{
    mJSON = nlohmann::json::array({std::move(json)});
    mIterator = mJSON.begin();
    mIsKey = false;
}

bool JSONDeserializer::decompose(const Type& type, void* value)
{
    if (mIsKey)
    {
        if (!type.has<StringConversionTrait>())
        {
            // JSON doesn't support structured keys, as JSON keys are just strings
            CUBOS_WARN("Type {} cannot be deserialized from a dictionary key - try defining a hook", type.name());
            return false;
        }

        const auto& trait = type.get<StringConversionTrait>();
        if (!trait.from(value, mIterator.key()))
        {
            CUBOS_WARN("Key {} is not a valid string representation of {}", mIterator.key(), type.name());
            return false;
        }

        return true;
    }

    if (mIterator->is_string() && type.has<StringConversionTrait>())
    {
        const auto& trait = type.get<StringConversionTrait>();
        if (!trait.from(value, mIterator.value()))
        {
            CUBOS_WARN("String {} is not a valid string representation of {}",
                       static_cast<std::string>(mIterator.value()), type.name());
            return false;
        }

        return true;
    }

    if (mIterator->is_string() && type.has<EnumTrait>())
    {
        const auto& trait = type.get<EnumTrait>();
        if (!trait.contains(mIterator.value()))
        {
            CUBOS_WARN("{} is not a valid enum variant of {}", static_cast<std::string>(mIterator.value()),
                       type.name());
            return false;
        }

        trait.at(mIterator.value()).set(value);
        return true;
    }

    if (mIterator->is_object() && type.has<DictionaryTrait>())
    {
        const auto& trait = type.get<DictionaryTrait>();
        auto view = trait.view(value);

        if (view.length() > 0)
        {
            if (!trait.hasErase())
            {
                CUBOS_WARN("Dictionary type {} must be empty to be deserialized as it doesn't support erase",
                           type.name());
                return false;
            }

            view.clear();
        }

        if (!trait.keyType().has<ConstructibleTrait>())
        {
            CUBOS_WARN("Dictionary type {} cannot be deserialized because its key type {} isn't constructible",
                       type.name(), trait.keyType().name());
            return false;
        }

        auto key = AnyValue::defaultConstruct(trait.keyType());
        for (auto it = mIterator->begin(), end = mIterator->end(); it != end; ++it)
        {
            mIterator = it;
            mIsKey = true;
            if (!this->read(key.type(), key.get()))
            {
                CUBOS_WARN("Couldn't deserialize dictionary key {}", it.key());
                return false;
            }

            view.insertDefault(key.get());
            auto entry = view.find(key.get());
            CUBOS_ASSERT(entry != view.end()); // We just inserted the key...

            mIterator = it;
            mIsKey = false;
            if (!this->read(trait.valueType(), entry->value))
            {
                CUBOS_WARN("Couldn't deserialize dictionary value with key {}", it.key());
                return false;
            }
        }

        return true;
    }

    if (mIterator->is_array() && type.has<ArrayTrait>())
    {
        const auto& trait = type.get<ArrayTrait>();
        auto view = trait.view(value);

        if (mIterator->size() != view.length())
        {
            if (!trait.hasResize())
            {
                CUBOS_WARN("Array type {} isn't resizable and the JSON array size ({}) doesn't match the its size ({})",
                           type.name(), mIterator->size(), view.length());
                return false;
            }

            view.resize(mIterator->size());
        }

        auto it = mIterator->begin();
        for (std::size_t i = 0; i < view.length(); ++i, ++it)
        {
            mIterator = it;
            mIsKey = false;
            if (!this->read(trait.elementType(), view.get(i)))
            {
                CUBOS_WARN("Couldn't deserialize array element {}", i);
                return false;
            }
        }

        return true;
    }

    if (type.has<FieldsTrait>())
    {
        const auto& trait = type.get<FieldsTrait>();
        auto view = trait.view(value);

        if (trait.size() == 1)
        {
            // If there's a single field, read it directly.
            if (!this->read(trait.begin()->type(), view.begin()->value))
            {
                CUBOS_WARN("Couldn't deserialize wrapped field {}", trait.begin()->name());
                return false;
            }

            return true;
        }

        if (!mIterator->is_object())
        {
            CUBOS_WARN("Type {} must be deserialized from a JSON object, not a {}", type.name(),
                       mIterator->type_name());
            return false;
        }

        for (auto it = mIterator->begin(), end = mIterator->end(); it != end; ++it)
        {
            const auto* field = trait.field(it.key());
            if (field == nullptr)
            {
                CUBOS_WARN("Type {} has no such field {}", type.name(), it.key());
                return false;
            }

            mIterator = it;
            mIsKey = false;
            if (!this->read(field->type(), view.get(*field)))
            {
                CUBOS_WARN("Couldn't deserialize field {}", it.key());
                return false;
            }
        }

        return true;
    }

    CUBOS_WARN("Cannot deserialize type {} from a JSON {}", type.name(), mIterator->type_name());
    return false;
}
