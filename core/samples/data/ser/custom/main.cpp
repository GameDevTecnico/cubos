#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::memory::Stream;

/// [Include]
#include <cubos/core/data/ser/serializer.hpp>

using cubos::core::data::Serializer;
using cubos::core::reflection::Type;
/// [Include]

/// [Your own serializer]
class MySerializer : public Serializer
{
public:
    MySerializer();

protected:
    bool decompose(const Type& type, const void* value) override;
};
/// [Your own serializer]

/// [Setting up hooks]
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::reflection::reflect;

MySerializer::MySerializer()
{
    this->hook<int32_t>([](const int32_t& value) {
        Stream::stdOut.print(value);
        return true;
    });
}
/// [Setting up hooks]

/// [Decomposing types]
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::FieldsTrait;

bool MySerializer::decompose(const Type& type, const void* value)
{
    if (type.has<ArrayTrait>())
    {
        const auto& arrayTrait = type.get<ArrayTrait>();

        Stream::stdOut.put('[');
        for (const auto* element : arrayTrait.view(value))
        {
            if (!this->write(arrayTrait.elementType(), element))
            {
                return false;
            }
            Stream::stdOut.print(", ");
        }
        Stream::stdOut.put(']');

        return true;
    }
    /// [Decomposing types]

    /// [Decomposing types with fields]
    if (type.has<FieldsTrait>())
    {
        Stream::stdOut.put('{');
        for (const auto& [field, fieldValue] : type.get<FieldsTrait>().view(value))
        {
            Stream::stdOut.printf("{}: ", field->name());
            if (!this->write(field->type(), fieldValue))
            {
                return false;
            }
            Stream::stdOut.print(", ");
        }
        Stream::stdOut.put('}');

        return true;
    }

    CUBOS_WARN("Cannot decompose {}", type.name());
    return false;
}
/// [Decomposing types with fields]

/// [Usage]
#include <glm/vec3.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/vector.hpp>

int main()
{
    std::vector<glm::ivec3> vec{{1, 2, 3}, {4, 5, 6}};

    MySerializer ser{};
    ser.write(vec);
}
/// [Usage]

/// [Output]
// [{x: 1, y: 2, z: 3, }, {x: 4, y: 5, z: 6, }, ]
/// [Output]
