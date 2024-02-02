#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_IMPL(uuids::uuid)
{
    using namespace cubos::core::reflection;

    return Type::create("uuids::uuid")
        .with(ConstructibleTrait::typed<uuids::uuid>().withBasicConstructors().build())
        .with(StringConversionTrait{
            [](const void* instance) { return uuids::to_string(*static_cast<const uuids::uuid*>(instance)); },
            [](void* instance, const std::string& string) {
                if (auto result = uuids::uuid::from_string(string))
                {
                    *static_cast<uuids::uuid*>(instance) = *result;
                    return true;
                }

                return false;
            }});
}
