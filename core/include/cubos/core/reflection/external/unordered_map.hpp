/// @file
/// @brief Declares `std::unordered_map<K, V>` as reflectable.

#pragma once

#include <unordered_map>

#include <cubos/core/reflection/dictionary.hpp>

/// @brief Define reflection for all `std::unordered_map<K, V>` types where both `K` and `V` are
/// reflectable.
CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename K, typename V), (std::unordered_map<K, V>))
{
    using Iterator = typename std::unordered_map<K, V>::iterator;

    std::string name = "std::unordered_map<" + reflect<K>().name() + ", " + reflect<V>().name() + ">";
    return DictionaryType::build(std::move(name), reflect<K>(), reflect<V>())
        .length([](const void* dictionary) { return static_cast<const std::unordered_map<K, V>*>(dictionary)->size(); })
        .value([](const void* dictionary, const void* key) {
            auto& map = *static_cast<const std::unordered_map<K, V>*>(dictionary);
            auto it = map.find(*static_cast<const K*>(key));
            return it == map.end() ? 0 : reinterpret_cast<uintptr_t>(&it->second);
        })
        .insert([](void* dictionary, const void* key) -> void* {
            auto& map = *static_cast<std::unordered_map<K, V>*>(dictionary);
            auto it = map.find(*static_cast<const K*>(key));
            if (it == map.end())
            {
                return &map.emplace(*static_cast<const K*>(key), V{}).first->second;
            }
            else
            {
                return &it->second;
            }
        })
        .remove([](void* dictionary, const void* key) {
            auto& map = *static_cast<std::unordered_map<K, V>*>(dictionary);
            return map.erase(*static_cast<const K*>(key)) == 1;
        })
        .iteratorNew([](const void* dictionary) -> void* {
            auto& map = *static_cast<std::unordered_map<K, V>*>(const_cast<void*>(dictionary));
            if (map.empty())
            {
                return nullptr;
            }

            return new Iterator(map.begin());
        })
        .iteratorKey([](const void*, const void* iterator) -> const void* {
            auto& it = *static_cast<const Iterator*>(iterator);
            return &it->first;
        })
        .iteratorValue([](const void*, const void* iterator) {
            auto& it = *static_cast<const Iterator*>(iterator);
            return reinterpret_cast<uintptr_t>(&it->second);
        })
        .iteratorIncrement([](const void* dictionary, void* iterator) {
            auto& map = *static_cast<std::unordered_map<K, V>*>(const_cast<void*>(dictionary));
            auto& it = *static_cast<Iterator*>(iterator);
            ++it;

            if (it == map.end())
            {
                delete &it;
                return true;
            }

            return false;
        })
        .iteratorDelete([](void* iterator) { delete static_cast<Iterator*>(iterator); })
        .template defaultConstructible<std::unordered_map<K, V>>()
        .get();
}
