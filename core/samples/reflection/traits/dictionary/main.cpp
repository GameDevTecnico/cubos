#include <cubos/core/log.hpp>

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
    CUBOS_INFO("Dictionary with {} entries of key type {} and value type {}", dictionaryTrait.length(instance),
               dictionaryTrait.keyType().name(), dictionaryTrait.valueType().name());
    /// [Getting dictionary length and types]

    /// [Getting dictionary entries]
    if (!dictionaryTrait.keyType().is<int32_t>() || !dictionaryTrait.valueType().is<int32_t>())
    {
        CUBOS_INFO("This function does not support printing dictionary with key and value types other than int");
        return;
    }

    for (auto it = dictionaryTrait.begin(instance); !it.isNull(); it.advance())
    {
        CUBOS_INFO("{} -> {}", *static_cast<const int32_t*>(it.key()), *static_cast<const int32_t*>(it.value()));
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