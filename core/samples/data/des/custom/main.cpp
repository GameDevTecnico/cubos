#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/tel/logging.hpp>

using cubos::core::memory::Stream;

/// [Include]
#include <cubos/core/data/des/deserializer.hpp>

using cubos::core::data::Deserializer;
using cubos::core::reflection::Type;
/// [Include]

/// [Your own deserializer]
class MyDeserializer : public Deserializer
{
public:
    MyDeserializer();

protected:
    bool decompose(const Type& type, void* value) override;
};
/// [Your own deserializer]

/// [Setting up hooks]
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::reflection::reflect;

MyDeserializer::MyDeserializer()
{
    this->hook<int32_t>([](int32_t& value) {
        Stream::stdOut.print("enter an int32_t: ");
        Stream::stdIn.parse(value);
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

bool MyDeserializer::decompose(const Type& type, void* value)
{
    if (type.has<ArrayTrait>())
    {
        const auto& arrayTrait = type.get<ArrayTrait>();
        auto arrayView = arrayTrait.view(value);

        auto length = static_cast<uint64_t>(arrayView.length());
        Stream::stdOut.printf("enter array size: ", length);
        Stream::stdIn.parse(length);

        for (std::size_t i = 0; i < static_cast<std::size_t>(length); ++i)
        {
            if (i == arrayView.length())
            {
                arrayView.insertDefault(i);
            }

            Stream::stdOut.printf("writing array[{}]: ", i);
            this->read(arrayTrait.elementType(), arrayView.get(i));
        }

        while (arrayView.length() > static_cast<std::size_t>(length))
        {
            arrayView.erase(static_cast<std::size_t>(length));
        }

        return true;
    }
    /// [Decomposing types]

    /// [Decomposing types with fields]
    if (type.has<FieldsTrait>())
    {
        for (const auto& [field, fieldValue] : type.get<FieldsTrait>().view(value))
        {
            Stream::stdOut.printf("writing field '{}': ", field->name());
            if (!this->read(field->type(), fieldValue))
            {
                return false;
            }
        }

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
    std::vector<glm::ivec3> vec{};
    MyDeserializer des{};
    des.read(vec);

    Stream::stdOut.print("-----------\n");
    Stream::stdOut.print("Resulting vec: [ ");
    for (const auto& v : vec)
    {
        Stream::stdOut.printf("({}, {}, {}) ", v.x, v.y, v.z);
    }
    Stream::stdOut.print("]\n");
}
/// [Usage]
