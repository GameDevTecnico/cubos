/// @file
/// @brief Reflection declaration for `std::unordered_map`.
/// @ingroup core-reflection

#pragma once

#include <unordered_map>

#include <cubos/core/reflection/traits/constructible_utils.hpp>
#include <cubos/core/reflection/traits/dictionary.hpp>
#include <cubos/core/reflection/type.hpp>

CUBOS_REFLECT_EXTERNAL_TEMPLATE((typename K, typename V, typename H, typename E), (std::unordered_map<K, V, H, E>))
{
    using Map = std::unordered_map<K, V, H, E>;

    auto dictionaryTrait = DictionaryTrait(
        reflect<K>(), reflect<V>(),
        [](const void* instance) -> std::size_t { return static_cast<const Map*>(instance)->size(); } /*length*/,
        [](uintptr_t instance, bool writeable) -> void* {
            if (writeable)
            {
                auto it = reinterpret_cast<Map*>(instance)->begin();
                if (it == reinterpret_cast<Map*>(instance)->end())
                {
                    return nullptr;
                }
                return new typename Map::iterator(it);
            }

            auto it = reinterpret_cast<const Map*>(instance)->cbegin();
            if (it == reinterpret_cast<const Map*>(instance)->cend())
            {
                return nullptr;
            }
            return new typename Map::const_iterator(it);
        } /*begin*/,
        [](uintptr_t instance, const void* key, bool writeable) -> void* {
            if (writeable)
            {
                auto it = reinterpret_cast<Map*>(instance)->find(*reinterpret_cast<const K*>(key));
                if (it == reinterpret_cast<Map*>(instance)->end())
                {
                    return nullptr;
                }
                return new typename Map::iterator(it);
            }

            auto it = reinterpret_cast<const Map*>(instance)->find(*reinterpret_cast<const K*>(key));
            if (it == reinterpret_cast<const Map*>(instance)->cend())
            {
                return nullptr;
            }
            return new typename Map::const_iterator(it);
        } /*find*/,
        [](uintptr_t instance, void* iterator, bool writeable) {
            if (writeable)
            {
                ++*static_cast<typename Map::iterator*>(iterator);
                return *static_cast<typename Map::iterator*>(iterator) != reinterpret_cast<Map*>(instance)->end();
            }

            ++*static_cast<typename Map::const_iterator*>(iterator);
            return *static_cast<typename Map::const_iterator*>(iterator) !=
                   reinterpret_cast<const Map*>(instance)->cend();
        } /*advance*/,
        [](void* iterator, bool writeable) {
            if (writeable)
            {
                delete static_cast<typename Map::iterator*>(iterator);
            }
            else
            {
                delete static_cast<typename Map::const_iterator*>(iterator);
            }
        } /*stop*/,
        [](const void* iterator, bool writeable) -> const void* {
            if (writeable)
            {
                return &(*static_cast<const typename Map::iterator*>(iterator))->first;
            }

            return &(*static_cast<const typename Map::const_iterator*>(iterator))->first;
        } /*key*/,
        [](const void* iterator, bool writeable) -> uintptr_t {
            if (writeable)
            {
                return reinterpret_cast<uintptr_t>(&(*static_cast<const typename Map::iterator*>(iterator))->second);
            }

            return reinterpret_cast<uintptr_t>(&(*static_cast<const typename Map::const_iterator*>(iterator))->second);
        } /*value*/);

    // We supply the insert functions depending on the constructibility of the value type.

    if constexpr (std::is_default_constructible<V>::value)
    {
        dictionaryTrait.setInsertDefault([](void* instance, const void* key) {
            auto* map = static_cast<Map*>(instance);
            map->emplace(std::piecewise_construct, std::make_tuple(*static_cast<const K*>(key)), std::make_tuple());
        });
    }

    if constexpr (std::is_copy_constructible<V>::value)
    {
        dictionaryTrait.setInsertCopy([](void* instance, const void* key, const void* value) {
            auto* map = static_cast<Map*>(instance);
            map->emplace(*static_cast<const K*>(key), *static_cast<const V*>(value));
        });
    }

    // Since V must be moveable for all std::unordered_map<K, V>, we always supply this function.
    dictionaryTrait.setInsertMove([](void* instance, const void* key, void* value) {
        auto* map = static_cast<Map*>(instance);
        map->emplace(*static_cast<const K*>(key), std::move(*static_cast<V*>(value)));
    });

    dictionaryTrait.setErase([](void* instance, const void* iterator) {
        static_cast<Map*>(instance)->erase(*static_cast<const typename Map::iterator*>(iterator));
    });

    return Type::create("std::unordered_map<" + reflect<K>().name() + ", " + reflect<V>().name() + ">")
        .with(dictionaryTrait)
        .with(autoConstructibleTrait<std::unordered_map<K, V>>());
}
