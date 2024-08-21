#include "type_client_server_defaults.hpp"
#include <memory>

#include <cubos/core/memory/any_vector.hpp>
#include <cubos/core/reflection/comparison.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/inherits.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type_client.hpp>
#include <cubos/core/reflection/type_server.hpp>

using cubos::core::memory::AnyValue;
using cubos::core::memory::AnyVector;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

namespace
{
    using FieldsValue = std::unordered_map<std::string, AnyValue>; // Maps field names to values.

    // Extracts the address of a field from a FieldsValue.
    class FieldsTraitAddressOf : public FieldsTrait::AddressOf
    {
    public:
        FieldsTraitAddressOf(std::string name)
            : mName(std::move(name))
        {
        }

        uintptr_t get(const void* instance) const override
        {
            const auto* value = static_cast<const FieldsValue*>(instance);
            return reinterpret_cast<uintptr_t>(value->at(mName).get());
        }

    private:
        std::string mName;
    };

    using ArrayValue = AnyVector; // Vector of values.

    struct DictionaryValue
    {
        AnyVector keys;
        AnyVector values;
    };

    struct DictionaryIterator
    {
        const DictionaryValue* dictionary;
        std::size_t index;
    };
} // namespace

void cubos::core::reflection::setupTypeClientDefaults(TypeClient& client)
{
    // Add all primitive types as known types.
    client.addType<bool>();
    client.addType<char>();
    client.addType<signed char>();
    client.addType<short>();
    client.addType<int>();
    client.addType<long>();
    client.addType<long long>();
    client.addType<unsigned char>();
    client.addType<unsigned short>();
    client.addType<unsigned int>();
    client.addType<unsigned long>();
    client.addType<unsigned long long>();
    client.addType<float>();
    client.addType<double>();

    // std::string is also included for convenience.
    client.addType<std::string>();

    // Add structural traits.
    client.addTrait<FieldsTrait>(
        /*isStructural=*/true,
        [](const TypeRegistry& registry, Type& type, data::Deserializer& des) {
            std::size_t numFields{};
            if (!des.read(numFields))
            {
                CUBOS_WARN("Could not deserialize FieldsTrait number of fields");
                return false;
            }

            FieldsTrait trait{};
            for (std::size_t i = 0; i < numFields; ++i)
            {
                std::string fieldName{};
                std::string typeName{};
                if (!des.read(fieldName) || !des.read(typeName))
                {
                    CUBOS_WARN("Could not deserialize FieldsTrait field name and type");
                    return false;
                }

                if (!registry.contains(typeName))
                {
                    CUBOS_WARN("Could not find field type {} for field {} of type {}", typeName, fieldName,
                               type.name());
                    return false;
                }

                const auto& fieldType = registry.at(typeName);
                if (fieldType.has<ConstructibleTrait>() && fieldType.get<ConstructibleTrait>().hasDefaultConstruct())
                {
                    trait.addField(fieldType, fieldName, new FieldsTraitAddressOf(fieldName));
                }
                else
                {
                    CUBOS_WARN("Field {} in type {} received from server does not have a "
                               "structural trait",
                               fieldName, typeName);
                    return false;
                }
            }

            FieldsValue defaultValue{};
            for (const auto& field : trait)
            {
                defaultValue.emplace(field.name(), AnyValue::defaultConstruct(field.type()));
            }

            type.with(memory::move(trait));
            type.with(TypeClient::RemoteTrait::createCustom<FieldsValue>(
                [defaultValue](void* instance) { new (instance) FieldsValue{defaultValue}; }));

            return true;
        },
        [](const Type& type, memory::Function<void(const Type&) const>& addType) {
            for (const auto& field : type.get<FieldsTrait>())
            {
                addType(field.type());
            }
        });

    client.addTrait<ArrayTrait>(
        /*isStructural=*/true,
        [](const TypeRegistry& registry, Type& type, data::Deserializer& des) {
            std::string elementTypeName{};
            if (!des.read(elementTypeName))
            {
                CUBOS_WARN("Could not deserialize ArrayTrait element type");
                return false;
            }

            if (!registry.contains(elementTypeName))
            {
                CUBOS_WARN("Could not find element type {} for ArrayTrait of type {}", elementTypeName, type.name());
                return false;
            }

            const auto& elementType = registry.at(elementTypeName);

            ArrayTrait trait{
                elementType,
                [](const void* instance) -> std::size_t { return static_cast<const ArrayValue*>(instance)->size(); },
                [](const void* instance, std::size_t index) -> std::uintptr_t {
                    return reinterpret_cast<std::uintptr_t>(static_cast<const ArrayValue*>(instance)->at(index));
                }};

            if (elementType.has<ConstructibleTrait>() && elementType.get<ConstructibleTrait>().hasDefaultConstruct())
            {
                trait.setInsertDefault([](void* instance, std::size_t index) {
                    static_cast<ArrayValue*>(instance)->insertDefault(index);
                });
            }

            if (elementType.has<ConstructibleTrait>() && elementType.get<ConstructibleTrait>().hasCopyConstruct())
            {
                trait.setInsertCopy([](void* instance, std::size_t index, const void* value) {
                    static_cast<ArrayValue*>(instance)->insertCopy(index, value);
                });
            }

            if (elementType.has<ConstructibleTrait>() && elementType.get<ConstructibleTrait>().hasMoveConstruct())
            {
                trait.setInsertMove([](void* instance, std::size_t index, void* value) {
                    static_cast<ArrayValue*>(instance)->insertMove(index, value);
                });
            }
            else
            {
                CUBOS_WARN("Could not deserialize ArrayTrait as element type {} is not move constructible",
                           elementType.name());
                return false;
            }

            trait.setErase([](void* instance, std::size_t index) { static_cast<ArrayValue*>(instance)->erase(index); });

            type.with(memory::move(trait));
            type.with(TypeClient::RemoteTrait::createCustom<ArrayValue>(
                [&elementType](void* instance) { new (instance) ArrayValue{elementType}; }));

            return true;
        },
        [](const Type& type, memory::Function<void(const Type&) const>& addType) {
            addType(type.get<ArrayTrait>().elementType());
        });

    client.addTrait<DictionaryTrait>(
        /*isStructural=*/true,
        [](const TypeRegistry& registry, Type& type, data::Deserializer& des) {
            std::string keyTypeName{};
            std::string valueTypeName{};
            if (!des.read(keyTypeName) || !des.read(valueTypeName))
            {
                CUBOS_WARN("Could not deserialize DictionaryTrait key and value types");
                return false;
            }

            if (!registry.contains(keyTypeName) || !registry.contains(valueTypeName))
            {
                CUBOS_WARN("Could not find key or value type for DictionaryTrait of type {}", type.name());
                return false;
            }

            const auto& keyType = registry.at(keyTypeName);
            const auto& valueType = registry.at(valueTypeName);

            if (!keyType.has<ConstructibleTrait>() || !keyType.get<ConstructibleTrait>().hasCopyConstruct())
            {
                CUBOS_WARN("Could not deserialize DictionaryTrait as key type {} is not copy constructible",
                           keyType.name());
                return false;
            }

            DictionaryTrait trait{
                keyType,
                valueType,
                [](const void* instance) -> std::size_t {
                    return static_cast<const DictionaryValue*>(instance)->keys.size();
                } /*length*/,
                [](uintptr_t instance, bool) -> void* {
                    const auto* value = reinterpret_cast<const DictionaryValue*>(instance);
                    if (value->keys.empty())
                    {
                        return nullptr;
                    }
                    return new DictionaryIterator{.dictionary = value, .index = 0};
                } /*begin*/,
                [](uintptr_t instance, const void* key, bool) -> void* {
                    const auto* value = reinterpret_cast<const DictionaryValue*>(instance);
                    for (std::size_t i = 0; i < value->keys.size(); ++i)
                    {
                        if (compare(value->keys.elementType(), value->keys.at(i), key))
                        {
                            return new DictionaryIterator{.dictionary = value, .index = i};
                        }
                    }
                    return nullptr;
                } /*find*/,
                [](uintptr_t, void* iterator, bool) -> bool {
                    auto* it = static_cast<DictionaryIterator*>(iterator);
                    ++it->index;
                    return it->index < it->dictionary->keys.size();
                } /*advance*/,
                [](void* iterator, bool) { delete static_cast<DictionaryIterator*>(iterator); } /*stop*/,
                [](const void* iterator, bool) -> const void* {
                    const auto* it = static_cast<const DictionaryIterator*>(iterator);
                    return it->dictionary->keys.at(it->index);
                } /*key*/,
                [](const void* iterator, bool) -> std::uintptr_t {
                    const auto* it = static_cast<const DictionaryIterator*>(iterator);
                    return reinterpret_cast<std::uintptr_t>(it->dictionary->values.at(it->index));
                } /*value*/,
            };

            if (valueType.has<ConstructibleTrait>() && valueType.get<ConstructibleTrait>().hasDefaultConstruct())
            {
                trait.setInsertDefault([](void* instance, const void* key) {
                    auto* dictionary = static_cast<DictionaryValue*>(instance);
                    for (std::size_t i = 0; i < dictionary->keys.size(); ++i)
                    {
                        if (compare(dictionary->keys.elementType(), dictionary->keys.at(i), key))
                        {
                            dictionary->values.setDefault(i);
                            return;
                        }
                    }

                    dictionary->keys.pushCopy(key);
                    dictionary->values.pushDefault();
                });
            }

            if (valueType.has<ConstructibleTrait>() && valueType.get<ConstructibleTrait>().hasCopyConstruct())
            {
                trait.setInsertCopy([](void* instance, const void* key, const void* value) {
                    auto* dictionary = static_cast<DictionaryValue*>(instance);
                    for (std::size_t i = 0; i < dictionary->keys.size(); ++i)
                    {
                        if (compare(dictionary->keys.elementType(), dictionary->keys.at(i), key))
                        {
                            dictionary->values.setCopy(i, value);
                            return;
                        }
                    }

                    dictionary->keys.pushCopy(key);
                    dictionary->values.pushCopy(value);
                });
            }

            if (valueType.has<ConstructibleTrait>() && valueType.get<ConstructibleTrait>().hasMoveConstruct())
            {
                trait.setInsertMove([](void* instance, const void* key, void* value) {
                    auto* dictionary = static_cast<DictionaryValue*>(instance);
                    for (std::size_t i = 0; i < dictionary->keys.size(); ++i)
                    {
                        if (compare(dictionary->keys.elementType(), dictionary->keys.at(i), key))
                        {
                            dictionary->values.setMove(i, value);
                            return;
                        }
                    }

                    dictionary->keys.pushCopy(key);
                    dictionary->values.pushMove(value);
                });
            }

            trait.setErase([](void* instance, const void* key) {
                auto* dictionary = static_cast<DictionaryValue*>(instance);
                for (std::size_t i = 0; i < dictionary->keys.size(); ++i)
                {
                    if (compare(dictionary->keys.elementType(), dictionary->keys.at(i), key))
                    {
                        dictionary->keys.erase(i);
                        dictionary->values.erase(i);
                        return;
                    }
                }
            });

            type.with(memory::move(trait));
            type.with(TypeClient::RemoteTrait::createCustom<DictionaryValue>([&keyType, &valueType](void* instance) {
                new (instance) DictionaryValue{{keyType}, {valueType}};
            }));

            return true;
        },
        [](const Type& type, memory::Function<void(const Type&) const>& addType) {
            addType(type.get<DictionaryTrait>().keyType());
            addType(type.get<DictionaryTrait>().valueType());
        });

    client.addTrait<EnumTrait>(
        /*isStructural=*/true,
        [](const TypeRegistry&, Type& type, data::Deserializer& des) {
            std::vector<std::string> variants{};
            if (!des.read(variants))
            {
                CUBOS_WARN("Could not deserialize EnumTrait variants");
                return false;
            }

            EnumTrait trait{};
            for (int i = 0; i < static_cast<int>(variants.size()); ++i)
            {
                trait.addVariant(
                    variants[static_cast<std::size_t>(i)],
                    [i](const void* value) { return i == *static_cast<const int*>(value); },
                    [i](void* value) { *static_cast<int*>(value) = i; });
            }
            type.with(memory::move(trait));
            type.with(TypeClient::RemoteTrait::createBasic<int>());

            return true;
        },
        [](const Type&, memory::Function<void(const Type&) const>&) {});

    client.addTrait<MaskTrait>(
        /*isStructural=*/true,
        [](const TypeRegistry&, Type& type, data::Deserializer& des) {
            std::vector<std::string> bits{};
            if (!des.read(bits))
            {
                CUBOS_WARN("Could not deserialize MaskTrait bits");
                return false;
            }

            CUBOS_ASSERT(bits.size() <= 64, "TypeClient only supports MaskTrait's with up to 64 bits");

            MaskTrait trait{};
            for (uint64_t i = 0; i < static_cast<uint64_t>(bits.size()); ++i)
            {
                trait.addBit(
                    bits[static_cast<std::size_t>(i)],
                    [i](const void* value) {
                        return (*static_cast<const uint64_t*>(value) & (static_cast<uint64_t>(1) << i)) !=
                               static_cast<uint64_t>(0);
                    },
                    [i](void* value) { *static_cast<uint64_t*>(value) |= (static_cast<uint64_t>(1) << i); },
                    [i](void* value) { *static_cast<uint64_t*>(value) &= ~(static_cast<uint64_t>(1) << i); });
            }
            type.with(memory::move(trait));
            type.with(TypeClient::RemoteTrait::createBasic<uint64_t>());

            return true;
        },
        [](const Type&, memory::Function<void(const Type&) const>&) {});

    client.addTrait<StringConversionTrait>(
        /*isStructural=*/true,
        [](const TypeRegistry&, Type& type, data::Deserializer&) {
            type.with(StringConversionTrait{
                [](const void* instance) -> std::string { return *static_cast<const std::string*>(instance); },
                [](void* instance, const std::string& str) -> bool {
                    // TODO: (issue #1316) communicate with the server to check if the string is valid?
                    *static_cast<std::string*>(instance) = str;
                    return true;
                }});
            type.with(TypeClient::RemoteTrait::createBasic<std::string>());

            return true;
        },
        [](const Type&, memory::Function<void(const Type&) const>&) {});

    // Add non-structural traits.

    client.addTrait<InheritsTrait>(
        /*isStructural=*/false,
        [](const TypeRegistry& registry, Type& type, data::Deserializer& des) {
            std::string baseTypeName{};
            if (!des.read(baseTypeName))
            {
                CUBOS_WARN("Could not deserialize InheritsTrait base type");
                return false;
            }

            if (!registry.contains(baseTypeName))
            {
                CUBOS_WARN("Could not find base type {} for InheritsTrait", baseTypeName);
                return false;
            }

            type.with(InheritsTrait{registry.at(baseTypeName)});

            return true;
        },
        [](const Type& type, memory::Function<void(const Type&) const>& addType) {
            addType(type.get<InheritsTrait>().base());
        });
}

void cubos::core::reflection::setupTypeServerDefaults(TypeServer& server)
{
    // Add all primitive types as known types.
    server.addType<bool>();
    server.addType<char>();
    server.addType<signed char>();
    server.addType<short>();
    server.addType<int>();
    server.addType<long>();
    server.addType<long long>();
    server.addType<unsigned char>();
    server.addType<unsigned short>();
    server.addType<unsigned int>();
    server.addType<unsigned long>();
    server.addType<unsigned long long>();
    server.addType<float>();
    server.addType<double>();

    // std::string is also included for convenience.
    server.addType<std::string>();

    // Add structural traits.
    server.addTrait<FieldsTrait>(
        [](data::Serializer& ser, const Type& type, bool) {
            if (!ser.write(type.get<FieldsTrait>().size()))
            {
                return false;
            }

            for (const auto& field : type.get<FieldsTrait>())
            {
                if (!ser.write(field.name()) || !ser.write(field.type().name()))
                {
                    return false;
                }
            }

            return true;
        },
        [](const Type& type, memory::Function<void(const Type&) const>& addType) {
            for (const auto& field : type.get<FieldsTrait>())
            {
                addType(field.type());
            }
        });

    server.addTrait<ArrayTrait>([](data::Serializer& ser, const Type& type,
                                   bool) { return ser.write(type.get<ArrayTrait>().elementType().name()); },
                                [](const Type& type, memory::Function<void(const Type&) const>& addType) {
                                    addType(type.get<ArrayTrait>().elementType());
                                });

    server.addTrait<DictionaryTrait>(
        [](data::Serializer& ser, const Type& type, bool) {
            const auto& trait = type.get<DictionaryTrait>();
            return ser.write(trait.keyType().name()) && ser.write(trait.valueType().name());
        },
        [](const Type& type, memory::Function<void(const Type&) const>& addType) {
            addType(type.get<DictionaryTrait>().keyType());
            addType(type.get<DictionaryTrait>().valueType());
        });

    server.addTrait<EnumTrait>(
        [](data::Serializer& ser, const Type& type, bool) {
            const auto& trait = type.get<EnumTrait>();
            std::vector<std::string> variants{};
            for (const auto& variant : trait)
            {
                variants.push_back(variant.name());
            }
            return ser.write(variants);
        },
        [](const Type&, memory::Function<void(const Type&) const>&) {});

    server.addTrait<MaskTrait>(
        [](data::Serializer& ser, const Type& type, bool) {
            const auto& trait = type.get<MaskTrait>();
            std::vector<std::string> bits{};
            for (const auto& bit : trait)
            {
                bits.push_back(bit.name());
            }
            return ser.write(bits);
        },
        [](const Type&, memory::Function<void(const Type&) const>&) {});

    server.addTrait<StringConversionTrait>([](data::Serializer&, const Type&, bool) { return true; },
                                           [](const Type&, memory::Function<void(const Type&) const>&) {});

    // Add non-structural traits.

    server.addTrait<InheritsTrait>(
        [](data::Serializer& ser, const Type& type, bool) {
            const auto& trait = type.get<InheritsTrait>();
            return ser.write(trait.base().name());
        },
        [](const Type& type, memory::Function<void(const Type&) const>& addType) {
            addType(type.get<InheritsTrait>().base());
        });
}
