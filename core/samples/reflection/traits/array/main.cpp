#include <cubos/core/log.hpp>

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
    if (!arrayTrait.elementType().is<int>())
    {
        CUBOS_INFO("This function does not support printing arrays of types other than int");
        return;
    }

    for (std::size_t i = 0; i < arrayView.length(); ++i)
    {
        CUBOS_INFO("Element {}: {}", i, *static_cast<const int*>(arrayView.get(i)));
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
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

int main()
{
    std::vector<int32_t> vec = {1, 1, 2, 3, 5, 8, 13};
    printArray(vec);

    /// [Expected output]
    // Array with 7 elements of type int32_t
    // Element 0: 1
    // Element 1: 1
    // Element 2: 2
    // Element 3: 3
    // Element 4: 5
    // Element 5: 8
    // Element 6: 13
    /// [Expected output]
}
/// [Usage]
