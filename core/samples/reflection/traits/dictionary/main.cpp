#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/tel/logging.hpp>

/// [Printing any dictionary]
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::DictionaryTrait;
using cubos::core::reflection::Type;

void printDictionary(const Type& type, const void* instance)
{
    const auto& dictionaryTrait = type.get<DictionaryTrait>();
    /// [Printing any dictionary]

    /// [Getting dictionary length and types]
    auto dictionaryView = dictionaryTrait.view(instance);
    CUBOS_INFO("Dictionary with {} entries of key type {} and value type {}", dictionaryView.length(),
               dictionaryTrait.keyType().name(), dictionaryTrait.valueType().name());
    /// [Getting dictionary length and types]

    /// [Getting dictionary entries]
    if (!dictionaryTrait.keyType().is<int32_t>() || !dictionaryTrait.valueType().is<int32_t>())
    {
        CUBOS_INFO("This function does not support printing dictionary with key and value types other than int32_t");
        return;
    }

    for (auto [key, value] : dictionaryView)
    {
        CUBOS_INFO("{} -> {}", *static_cast<const int32_t*>(key), *static_cast<const int32_t*>(value));
    }
}
/// [Getting dictionary entries]

/// [Typed wrapper]
template <typename T>
void printDictionary(const T& dictionary)
{
    using cubos::core::reflection::reflect;

    printDictionary(reflect<T>(), &dictionary);
}
/// [Typed wrapper]

/// [Usage]
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

int main()
{
    std::map<int32_t, int32_t> map = {
        {1, 2},
        {2, 4},
        {3, 6},
        {4, 8},
    };
    printDictionary(map);

    /// [Expected output]
    // Dictionary with 4 entries of key type int32_t and value type int32_t
    // 1 -> 2
    // 2 -> 4
    // 3 -> 6
    // 4 -> 8
    /// [Expected output]
}
/// [Usage]
