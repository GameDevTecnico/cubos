#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/tel/logging.hpp>

/// [Printing any array]
#include <cubos/core/reflection/traits/array.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::ArrayTrait;
using cubos::core::reflection::Type;

void printArray(const Type& type, const void* instance)
{
    const auto& arrayTrait = type.get<ArrayTrait>();
    /// [Printing any array]

    /// [Getting array length and type]
    auto arrayView = arrayTrait.view(instance);
    CUBOS_INFO("Array with {} elements of type {}", arrayView.length(), arrayTrait.elementType().name());
    /// [Getting array length and type]

    /// [Getting array elements]
    if (!arrayTrait.elementType().is<int32_t>())
    {
        CUBOS_INFO("This function does not support printing arrays of types other than int32_t");
        return;
    }

    for (const auto* element : arrayView)
    {
        CUBOS_INFO("{}", *static_cast<const int32_t*>(element));
    }
}
/// [Getting array elements]

/// [Typed wrapper]
template <typename T>
void printArray(const T& array)
{
    using cubos::core::reflection::reflect;

    printArray(reflect<T>(), &array);
}
/// [Typed wrapper]

/// [Usage]
// You must also include <cubos/core/reflection/external/primitives.hpp> :)
#include <cubos/core/reflection/external/vector.hpp>

int main()
{
    std::vector<int32_t> vec = {1, 1, 2, 3, 5, 8, 13};
    printArray(vec);
}
/// [Usage]

/// [Expected output]
// Array with 7 elements of type int32_t
// 1
// 1
// 2
// 3
// 5
// 8
// 13
/// [Expected output]
