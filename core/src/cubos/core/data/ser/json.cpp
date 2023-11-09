#include <cubos/core/log.hpp>

/// [JSON serializer]
#include <cubos/core/data/ser/json.hpp>

using cubos::core::data::JSONSerializer;
using cubos::core::reflection::Type;

auto jsonVal = nlohmann::json::object(); ///< The current JSON value that is being serialized.

/// [MACRO to add hooks for primitive types]
#define AUTO_HOOK(type, ...)                                                                                           \
    this->hook<type>([](const type& value) {                                                                           \
        jsonVal = value;                                                                                               \
        return true;                                                                                                   \
    })

/// [Setting up hooks]
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::reflection::reflect;

JSONSerializer::JSONSerializer()
{
    AUTO_HOOK(bool);
    AUTO_HOOK(char);
    AUTO_HOOK(int8_t);
    AUTO_HOOK(int16_t);
    AUTO_HOOK(int32_t);
    AUTO_HOOK(int64_t);
    AUTO_HOOK(uint8_t);
    AUTO_HOOK(uint16_t);
    AUTO_HOOK(uint32_t);
    AUTO_HOOK(uint64_t);
    AUTO_HOOK(float);
    AUTO_HOOK(double);
    AUTO_HOOK(std::string);
}
/// [Setting up hooks]

nlohmann::json JSONSerializer::output()
{
    return mJSON;
}

/// [Decomposing types]
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::FieldsTrait;

bool JSONSerializer::decompose(const Type& type, const void* value)
{
    if (type.has<ArrayTrait>())
    {
        nlohmann::json jsonArr; ///< Hold current JSON array

        const auto& arrayTrait = type.get<ArrayTrait>();

        for (const auto* element : arrayTrait.view(value))
        {
            if (!this->write(arrayTrait.elementType(), element))
            {
                return false;
            }
            jsonArr.push_back(mJSON);
        }

        mJSON = jsonArr;
        return true;
    }
    /// [Decomposing types]

    /// [Decomposing types with fields]
    if (type.has<FieldsTrait>())
    {
        nlohmann::json jsonObj; ///< Hold current JSON object

        for (const auto& [field, fieldValue] : type.get<FieldsTrait>().view(value))
        {
            if (!this->write(field->type(), fieldValue))
            {
                return false;
            }
            jsonObj[field->name()] = jsonVal;
        }
        mJSON = jsonObj;
        return true;
    }

    CUBOS_WARN("Cannot decompose '{}'", type.name());
    return false;
}
/// [Decomposing types with fields]